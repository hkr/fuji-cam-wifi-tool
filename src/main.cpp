#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include "log.hpp"
#include "comm.hpp"
#include "commands.hpp"

#include <thread>
#include <chrono>
#include <vector>

namespace fcwt {

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

status_request_message generate_status_request_message()
{
  status_request_message msg = {};
  msg.id = generate_message_id();
  return msg;
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
  fuji_message(sockfd, generate_message_id(), shutter_message_1);
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
  sock const sockfd2 = connect_to_camera(jpg_stream_server_port);

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
}

int main()
{
  sock const sockfd = connect_to_camera(control_server_port);
  if (sockfd < 0)
    return 1;

  if (!init_control_connection(sockfd, "HackedClient"))
    return 1;
  
  std::atomic<bool> imageStreamFlag(true);
  std::thread imageStreamThread([&]() { image_stream_main(imageStreamFlag); });

  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  shutter(sockfd);

  std::this_thread::sleep_for(std::chrono::seconds(2));

  imageStreamFlag = false;
  imageStreamThread.join();

  return 0;
}

} // namespace fcwt

int main(const int argc, char const* argv[])
{
  return fcwt::main();
}
