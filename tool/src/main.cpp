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
      linenoiseHistoryAdd(line.c_str());
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
