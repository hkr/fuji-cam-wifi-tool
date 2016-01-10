#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include "log.hpp"
#include "comm.hpp"
#include "commands.hpp"

#include "linenoise/linenoise.h"

#include <thread>
#include <chrono>
#include <vector>
#include <string>

namespace fcwt {

static void print_status(int sockfd)
{
    auto const msg = generate<status_request_message>();
    fuji_send(sockfd, &msg, sizeof(msg));
    uint8_t buf[1024];
    uint32_t receivedBytes = fuji_receive(sockfd, buf);
    print_hex(buf, receivedBytes);
    receivedBytes = fuji_receive(sockfd, buf);
    print_hex(buf, receivedBytes);
}

bool shutter(int const sockfd) 
{
  if (sockfd <= 0)
    return false;

  LOG_INFO("shutter");
  fuji_message(sockfd, make_static_message(message_type::shutter, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00));
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

char const* comamndStrings[] =
{
  "connect",
  "shutter",
  "stream",
};

enum class command 
{
  connect,
  shutter,
  stream,
  unknown,
  count = unknown
};

static void completion(char const* buf, linenoiseCompletions* lc)
{
  for (int i = 0; i < static_cast<int>(command::count); ++i)
  {
    char const* const cmd = comamndStrings[i];
    if (strstr(cmd, buf) == cmd)
      linenoiseAddCompletion(lc, cmd);
  }
}

bool getline(std::string& line)
{
  char* const str = linenoise("fcwt> ");
  if (!str)
    return false;

  line.assign(str);
  free(str);
  return true;
}

command parse_command(std::string const& line)
{
  for (int i = 0; i < static_cast<int>(command::count); ++i)
  {
    if (line == comamndStrings[i])
      return static_cast<command>(i);
  }
  
  return command::unknown;
}

int main()
{

  /* Set the completion callback. This will be called every time the
   * user uses the <tab> key. */
  linenoiseSetCompletionCallback(completion);

  sock sockfd;
  std::atomic<bool> imageStreamFlag(true);
  std::thread imageStreamThread;

  std::string line;
  while(getline(line))
  {
      command cmd = parse_command(line);
      /* Do something with the string. */
      switch(cmd)
      {
        case command::connect:
        {
          if (sockfd <= 0)
          {
            sockfd = connect_to_camera(control_server_port);
            if (!init_control_connection(sockfd, "HackedClient"))
              printf("failure\n");
          }
          else
          {
            printf("already connected\n");
          }
        }
        break;
      case command::shutter:
      {
        if (!shutter(sockfd))
          printf("failure\n");
      }
      break;
      case command::stream:
      {
        imageStreamThread = std::thread(([&]() { image_stream_main(imageStreamFlag); }));
      }
      break;
      default:
      {
        printf("Unreconized command: %s\n", line.c_str());
      }
    }
  }

  if (imageStreamThread.joinable())
  {
    imageStreamFlag = false;
    imageStreamThread.join();
  }

  terminate_control_connection(sockfd);

  return 0;
}

} // namespace fcwt

int main(const int argc, char const* argv[])
{
  return fcwt::main();
}
