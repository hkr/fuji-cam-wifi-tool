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

uint8_t message1_response_error[] =
{
  0x05, 0x00, 0x00, 0x00, 0x19, 0x20, 0x00, 0x00
};

uint8_t message2[] =
{
  0x01, 0x00, 0x02, 0x10, 
  0x01, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00
};

uint8_t message2_response_success[] =
{
  0x03, 0x00, 0x01, 0x20,
  0x01, 0x00, 0x00, 0x00
};

uint8_t message3[] = 
{
  0x01, 0x00, 0x15, 0x10, 
  0x02, 0x00, 0x00, 0x00, 
  0x12, 0xd2, 0x00, 0x00
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
  0x01, 0x00, 0x16, 0x10, 0x05, 0x00, 0x00, 0x00, 0x24, 0xdf, 0x00, 0x00
};

uint8_t message6_2[] =
{
  0x02, 0x00, 0x16, 0x10, 0x05, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00
};

uint8_t message7[] =
{
  0x01, 0x00, 0x15, 0x10, 0x06, 0x00, 0x00, 0x00, 0x12, 0xd2, 0x00, 0x00
};

uint8_t message8[] =
{
  0x01, 0x00, 0x15, 0x10, 0x07, 0x00, 0x00, 0x00, 0x12, 0xd2, 0x00, 0x00
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

uint8_t message11[] =
{
  0x01, 0x00, 0x15, 0x10,
  0x0a, 0x00, 0x00, 0x00,
  0x12, 0xd2, 0x00, 0x00
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

static void login_sequence(int sockfd) {
  auto const reg_msg = generate_registration_message("HackedClient");
  fuji_send(sockfd, &reg_msg, sizeof(reg_msg));
  LOG_INFO("sent message 1");

  uint8_t buffer[1024 * 1024];
  uint32_t receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  print_hex(buffer, receivedBytes);
  if (receivedBytes == sizeof(message1_response_error) && memcmp(buffer, message1_response_error, receivedBytes) == 0)
  {
     fatal_error("response on message1, 0x error");
  }

  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
 
  //
  LOG_INFO("message2");
  fuji_send(sockfd, message2);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message3");
  fuji_send(sockfd, message3);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message4");
  fuji_send(sockfd, message4_1);
  fuji_send(sockfd, message4_2);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message5");
  fuji_send(sockfd, message5);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message6");
  fuji_send(sockfd, message6_1);
  fuji_send(sockfd, message6_2);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message7");
  fuji_send(sockfd, message7);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message8");
  fuji_send(sockfd, message8);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message9");
  fuji_send(sockfd, message9);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message10");
  fuji_send(sockfd, message10);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);

  //
  LOG_INFO("message11");
  fuji_send(sockfd, message10);
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
}

void shutter(int const sockfd) 
{
  LOG_INFO("shutter");
  uint8_t const shutter_message_1[] = { 0x01, 0x00, 0x15, 0x10, 0x14, 0x00, 0x00, 0x00, 0x12, 0xd2, 0x00, 0x00 };
  uint8_t const shutter_message_2[] = { 0x01, 0x00, 0x22, 0x90, 0x15, 0x00, 0x00, 0x00 };

  uint8_t buffer[1024 * 1024];
  uint32_t receivedBytes = 0;
#if 0
  LOG_INFO("shutter_message_1");
  fuji_send(sockfd, shutter_message_1);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
  print_hex(buffer, receivedBytes);
#endif
  LOG_INFO("shutter_message_2");
  fuji_send(sockfd, shutter_message_1);
  receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
  LOG_INFO_FORMAT("received %d bytes", receivedBytes);
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
  
// shutter(sockfd);

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
