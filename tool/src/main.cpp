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

static void print_status(int sockfd) {
  auto const msg = generate<status_request_message>();
  printf("Status request %d\n", msg.id);
  fuji_send(sockfd, &msg, sizeof(msg));
  uint8_t buf[1024];
  uint32_t receivedBytes = fuji_receive(sockfd, buf);
  printf("Status: %d bytes\n", receivedBytes);
  print_hex(buf, receivedBytes);
  print_uint32(buf, receivedBytes);
  print_ascii(buf, receivedBytes);

  receivedBytes = fuji_receive(sockfd, buf);
}

void image_stream_main(std::atomic<bool>& flag) {
  LOG_INFO("image_stream_main");
  sock const sockfd2 = connect_to_camera(jpg_stream_server_port);

  std::vector<uint8_t> buffer(1024 * 1024);

  if (sockfd2 <= 0) return;

  unsigned int image = 0;
  while (flag) {
    uint32_t receivedBytes =
        fuji_receive(sockfd2, buffer.data(), buffer.size());
    LOG_INFO_FORMAT("image_stream_main received %d bytes", receivedBytes);

    char filename[1024];
    snprintf(filename, sizeof(filename), "out/img_%d.jpg", image++);
    FILE* file = fopen(filename, "wb");
    if (file) {
      int const header = 14;  // not sure what's in the first 14 bytes
      fwrite(&buffer[header], receivedBytes, 1, file);
      fclose(file);
    } else {
      LOG_WARN_FORMAT("image_stream_main Failed to create file %s", filename);
    }
  }
}

char const* comamndStrings[] = {"connect", "shutter", "stream", "info",
                                "set_iso"};

enum class command {
  connect,
  shutter,
  stream,
  info,
  set_iso,
  unknown,
  count = unknown
};

static void completion(char const* buf, linenoiseCompletions* lc) {
  for (int i = 0; i < static_cast<int>(command::count); ++i) {
    char const* const cmd = comamndStrings[i];
    if (strstr(cmd, buf) == cmd) linenoiseAddCompletion(lc, cmd);
  }
}

bool getline(std::string& line) {
  char* const str = linenoise("fcwt> ");
  if (!str) return false;

  line.assign(str);
  free(str);
  return true;
}

command parse_command(std::string const& line) {
  for (int i = 0; i < static_cast<int>(command::count); ++i) {
    if (line == comamndStrings[i]) return static_cast<command>(i);
  }

  return command::unknown;
}

std::vector<std::string> split(std::string const& str,
                               int delimiter(int) = ::isspace) {
  std::vector<std::string> result;
  auto const itEnd = str.end();
  auto it = str.begin();
  while (it != itEnd) {
    it = std::find_if_not(it, itEnd, delimiter);
    if (it == itEnd) break;

    auto const it2 = std::find_if(it, itEnd, delimiter);
    result.emplace_back(it, it2);
    it = it2;
  }
  return result;
}

int main() {
  /* Set the completion callback. This will be called every time the
   * user uses the <tab> key. */
  linenoiseSetCompletionCallback(completion);

  sock sockfd;
  std::atomic<bool> imageStreamFlag(true);
  std::thread imageStreamThread;
  camera_capabilities caps = {};

  std::string line;
  while (getline(line)) {
    linenoiseHistoryAdd(line.c_str());
    auto const splitLine = split(line);
    if (splitLine.empty()) continue;

    command cmd = parse_command(splitLine[0]);
    switch (cmd) {
      case command::connect: {
        if (sockfd <= 0) {
          sockfd = connect_to_camera(control_server_port);
          if (!init_control_connection(sockfd, "HackedClient", &caps))
            printf("failure\n");
          else {
            print(caps);
            print_status(sockfd);
          }
        } else {
          printf("already connected\n");
        }
      } break;
      case command::shutter: {
        if (!shutter(sockfd)) printf("failure\n");
        print_status(sockfd);

      } break;

      case command::stream: {
        imageStreamThread =
            std::thread(([&]() { image_stream_main(imageStreamFlag); }));
      } break;

      case command::info: {
        print_status(sockfd);
        camera_settings settings;
        if (current_settings(sockfd, settings))
          print(settings);
      } break;

      case command::set_iso: {
        if (splitLine.size() > 1) {
          unsigned long iso = std::stoul(splitLine[1]);
          printf("%s(%lu)\n", splitLine[0].c_str(), iso);
          if (set_iso(sockfd, iso)) {
            print_status(sockfd);
          } else {
            printf("Failed to set ISO %lu\n", iso);
          }
        }
      } break;

      default: { printf("Unreconized command: %s\n", line.c_str()); }
    }
  }

  if (imageStreamThread.joinable()) {
    imageStreamFlag = false;
    imageStreamThread.join();
  }

  terminate_control_connection(sockfd);

  return 0;
}

}  // namespace fcwt

int main(const int argc, char const* argv[]) { return fcwt::main(); }
