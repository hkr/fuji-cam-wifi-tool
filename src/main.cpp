#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include "log.hpp"
#include "comm.hpp"

#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

namespace fcwt {

struct registration_message
{
  uint8_t const header[24] =
  {
    0x01, 0x00, 0x00, 0x00, 0xf2, 0xe4, 0x53, 0x8f,
    0xad, 0xa5, 0x48, 0x5d, 0x87, 0xb2, 0x7f, 0x0b, 
    0xd3, 0xd5, 0xde, 0xd0, 0x00, 0x00, 0x00, 0x00
  };
  uint8_t device_name[54] = {};
};

struct message
{
  uint16_t index = 1;   // index if this is a multi-part message, not really surea bout the second byte
  uint8_t type[2];  // not sure about this, might be two separate fields
  uint32_t id;
};

struct status_request_message : message
{
  status_request_message()
  {
    index = 1;
    type[0] = 0x15;
    type[1] = 0x10;
    id = 0;
  }
  uint8_t const data[4] = { 0x12, 0xd2, 0x00, 0x00 };
};

struct response_success
{
  uint8_t const type[4] = {0x03, 0x00, 0x01, 0x20};
  uint32_t id;
};

uint8_t message1_response_error[] =
{
  0x05, 0x00, 0x00, 0x00,
  0x19, 0x20, 0x00, 0x00
};

uint8_t message2[] =
{
  0x01, 0x00, 0x02, 0x10, 
  0x01, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00
};

uint8_t message4_1[] =
{
  0x01, 0x00, 0x16, 0x10,
  0x03, 0x00, 0x00, 0x00,
  0x01, 0xdf, 0x00, 0x00
};

uint8_t message4_2[] =
{
   0x02, 0x00, 0x16, 0x10, 
   0x03, 0x00, 0x00, 0x00, 
   0x05, 0x00
};

uint8_t message5[] =
{
  0x01, 0x00, 0x15, 0x10,
  0x04, 0x00, 0x00, 0x00,
  0x24, 0xdf, 0x00, 0x00
};

uint8_t message6_1[] =
{
  0x01, 0x00, 0x16, 0x10,
  0x05, 0x00, 0x00, 0x00,
  0x24, 0xdf, 0x00, 0x00
};

uint8_t message6_2[] =
{
  0x02, 0x00, 0x16, 0x10,
  0x05, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x02, 0x00
};

uint8_t message9[] =
{
  0x01, 0x00, 0x2b, 0x90,
  0x08, 0x00, 0x00, 0x00
};

uint8_t message10[] =
{
  0x01, 0x00, 0x1c, 0x10,
  0x09, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

registration_message generate_registration_message(char const* device_name)
{
	registration_message msg = {};
	const int max_characters = sizeof(msg.device_name) / 2 - 1;
	for (int i = 0; i < max_characters; ++i) {
		const char c = device_name[i];
		msg.device_name[2 * i + 0] = c;
		msg.device_name[2 * i + 1] = 0;
		if (c == 0)
			break;
	}

	return msg;
}

static uint32_t generate_message_id()
{
  static std::atomic<uint32_t> id_counter;
  return ++id_counter;
}

template <size_t N>
uint8_t (&add_message_id(uint8_t(&buffer)[N], uint32_t id))[N]
{
  static_assert(N >= 8, "buffer must be larger than 8 bytes");
  memcpy(&buffer[4], &id, sizeof(id));
  return buffer;
} 

status_request_message generate_status_request_message()
{
  status_request_message msg = {};
  msg.id = generate_message_id();
  return msg;
}

static bool is_success_response(uint32_t const id, void const* buffer, uint32_t const size)
{
  if (size != 8)
    return false;
  
  response_success success = {};
  success.id = id;
  bool const result = memcmp(&success, buffer, 8) == 0;
  if (!result)
  {
    LOG_INFO("expected: ");
    print_hex(&success, 8);
    LOG_INFO("actual: ");
    print_hex(buffer, 8);
  }
  return result;
}

template <size_t N>
static bool fuji_send_and_receive(int const sockfd, uint32_t const id, uint8_t(&msg)[N])
{
  fuji_send(sockfd, add_message_id(msg, id));

  uint8_t buffer[1024];
  uint32_t receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  return is_success_response(id, buffer, receivedBytes);
}

template <size_t N1, size_t N2>
static bool fuji_send_and_receive(int const sockfd, uint32_t const id, uint8_t(&msg1)[N1], uint8_t(&msg2)[N2])
{
  fuji_send(sockfd, add_message_id(msg1, id));
  return fuji_send_and_receive(sockfd, id, msg2);
}

static void login_sequence(int const sockfd) {
  auto const reg_msg = generate_registration_message("HackedClient");
  fuji_send(sockfd, &reg_msg, sizeof(reg_msg));
  LOG_INFO("sent message 1");

  uint8_t buffer[1024];
  uint32_t receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  print_hex(buffer, receivedBytes);
  if (receivedBytes == sizeof(message1_response_error) && memcmp(buffer, message1_response_error, receivedBytes) == 0)
  {
     fatal_error("response on message1, 0x error");
  }

  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
 
  //
  LOG_INFO("message2");
  fuji_send_and_receive(sockfd, generate_message_id(), message2);

  //
  LOG_INFO("message4");
  fuji_send_and_receive(sockfd, generate_message_id(), message4_1, message4_2);

  //
  LOG_INFO("message5");
  fuji_send(sockfd, add_message_id(message5, generate_message_id()));
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message6");
  fuji_send_and_receive(sockfd, generate_message_id(), message6_1, message6_2);

  //
  LOG_INFO("message9");
  fuji_send(sockfd, add_message_id(message9, generate_message_id()));
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message10");
  fuji_send_and_receive(sockfd, generate_message_id(), message10);
}

static void print_status(int sockfd)
{
    status_request_message msg = generate_status_request_message();
    fuji_send(sockfd, &msg, sizeof(msg));
    uint8_t buf[1024];
    uint32_t receivedBytes = fuji_receive(sockfd, buf);
    print_hex(buf, receivedBytes);
    receivedBytes = fuji_receive(sockfd, buf);
    print_hex(buf, receivedBytes);
}

bool shutter(int const sockfd) 
{
  LOG_INFO("shutter");
  uint8_t shutter_message_1[] =
  {
    0x01, 0x00, 0x0e, 0x10,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
  };
  fuji_send_and_receive(sockfd, generate_message_id(), shutter_message_1);
  return true;

  // Not sure why we don't get the image as response to shutter_message_2
  #if 0
  uint8_t buffer[20 * 1024];
  uint32_t receivedBytes = 0;
  message shutter_message_2;
  while (1) {


  LOG_INFO("shutter_message_2");
  message shutter_message_2;
  shutter_message_2.type[0] = 0x22;
  shutter_message_2.type[1] = 0x90;
  shutter_message_2.id = generate_message_id();
  fuji_send(sockfd, &shutter_message_2, sizeof(shutter_message_2));

  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  }
  return is_success_response(shutter_message_2.id, buffer, receivedBytes);
  #endif
}

void image_stream_main(std::atomic<bool>& flag)
{
  LOG_INFO("image_stream_main");
  int const sockfd2 = connect_to_camera(jpg_stream_server_port);


  std::vector<uint8_t> buffer(1024 * 1024);

  if (sockfd2 <= 0)
    return;

  unsigned int image = 0;
  while (flag)
  {
    uint32_t receivedBytes = fuji_receive(sockfd2, buffer.data(), buffer.size());
    LOG_INFO_FORMAT("image_stream_main received %d bytes", receivedBytes);

    char filename[1024];
    snprintf(filename, sizeof(filename), "out/img_%d.jpg", image++);
    FILE* file = fopen(filename, "wb");
    if (file)
    {
      int const header = 14; // not sure what's in the first 14 bytes
      fwrite(&buffer[header], receivedBytes, 1, file);
      fclose(file);
    }
    else
    {
      LOG_WARN_FORMAT("image_stream_main Failed to create file %s", filename);
    }
  }

  close(sockfd2);
}

int main()
{
  int const sockfd = connect_to_camera(main_server_port);
  if (sockfd < 0)
    return 1;

  login_sequence(sockfd);
  
  std::atomic<bool> imageStreamFlag(true);
  std::thread imageStreamThread([&]() { image_stream_main(imageStreamFlag); });

  std::this_thread::sleep_for(std::chrono::seconds(1));
  
 shutter(sockfd);

  std::this_thread::sleep_for(std::chrono::seconds(2));

  imageStreamFlag = false;
  imageStreamThread.join();

////
  if (sockfd > 0)
    close(sockfd);

  return 0;
}

} // namespace fcwt

int main(const int argc, char const* argv[])
{
  return fcwt::main();
}
