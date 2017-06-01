#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#include "log.hpp"
#include "comm.hpp"
#include "commands.hpp"

#include "linenoise.h"

#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <atomic>
#include <algorithm>

namespace fcwt {

void image_stream_main(std::atomic<bool>& flag) {
  LOG_INFO("image_stream_main");
  sock const sockfd3 = connect_to_camera(jpg_stream_server_port);

  std::vector<uint8_t> buffer(1024 * 1024);

  if (sockfd3 <= 0) return;

  unsigned int image = 0;
  while (flag) {
    size_t receivedBytes =
        fuji_receive(sockfd3, buffer.data(), buffer.size());
    LOG_INFO_FORMAT("image_stream_main received %zd bytes", receivedBytes);

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
                                "set_iso", "set_aperture", "aperture",
                                "shutter_speed", "set_shutter_speed",
                                "white_balance", "current_settings" };

enum class command {
  connect,
  shutter,
  stream,
  info,
  set_iso,
  set_aperture,
  aperture,
  shutter_speed,
  set_shutter_speed,
  white_balance,
  current_settings,
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
  sock sockfd2;
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
            camera_settings settings;
            if (current_settings(sockfd, settings))
              print(settings);
            sockfd2 = connect_to_camera(async_response_server_port);
          }
        } else {
          printf("already connected\n");
        }
      } break;
      case command::shutter: {
        if (!shutter(sockfd, sockfd2)) printf("failure\n");

      } break;

      case command::stream: {
        imageStreamThread =
            std::thread(([&]() { image_stream_main(imageStreamFlag); }));
      } break;

      case command::info: {
        camera_settings settings;
        if (current_settings(sockfd, settings))
          print(settings);
      } break;

      case command::set_iso: {
        if (splitLine.size() > 1) {
          unsigned long iso = std::stoul(splitLine[1], 0, 0);
          printf("%s(%lu)\n", splitLine[0].c_str(), iso);
          if (update_setting(sockfd, iso_level(iso))) {
            camera_settings settings;
            if (current_settings(sockfd, settings))
              print(settings);
          } else {
            printf("Failed to set ISO %lu\n", iso);
          }
        }
      } break;

      case command::set_aperture: {
        if (splitLine.size() > 1) {
          int const aperture = static_cast<int>(std::stod(splitLine[1]) * 100.0);
          camera_settings settings;
          if (aperture > 0 && aperture < 6400 && 
              current_settings(sockfd, settings) && settings.aperture.value > 0 &&
              aperture != settings.aperture.value) {
            const aperture_f_stop change = aperture < settings.aperture.value ? aperture_open_third_stop : aperture_close_third_stop;
            uint32_t last_aperture = 0;
            do {
              last_aperture = settings.aperture.value;
              if (!update_setting(sockfd, change))
                break;
            } while(current_settings(sockfd, settings) && 
                    settings.aperture.value != last_aperture && 
                    aperture != settings.aperture.value &&
                    change == (aperture < settings.aperture.value ? aperture_open_third_stop : aperture_close_third_stop));
            print(settings);
          } 
        }
      } break;

      case command::aperture: {
        if (splitLine.size() > 1) {
          int aperture_stops = std::stoi(splitLine[1], 0, 0);
          printf("%s(%i)\n", splitLine[0].c_str(), aperture_stops);
          if (aperture_stops != 0) {
            if (update_setting(sockfd, aperture_stops < 0 ? aperture_open_third_stop : aperture_close_third_stop)) {
              camera_settings settings;
              if (current_settings(sockfd, settings))
                print(settings);
            } else {
              printf("Failed to adjust aperture %i\n", aperture_stops);
            }
          }
        }
      } break;

      case command::shutter_speed: {
        if (splitLine.size() > 1) {
          int shutter_stops = std::stoi(splitLine[1], 0, 0);
          printf("%s(%i)\n", splitLine[0].c_str(), shutter_stops);
          if (shutter_stops != 0) {
            if (update_setting(sockfd, shutter_stops < 0 ? shutter_speed_one_stop_slower : shutter_speed_one_stop_faster)) {
              camera_settings settings;
              if (current_settings(sockfd, settings))
                print(settings);
            } else {
              printf("Failed to adjust shutter speed %i\n", shutter_stops);
            }
          }
        }
      } break;

      case command::set_shutter_speed: {
        if (splitLine.size() > 1) {
          std::size_t pos = 0;
          double nom, denom;
          int res = std::sscanf(splitLine[1].c_str(), "%lf/%lf", &nom, &denom);
          if (res > 0) {
            const uint64_t shutter_speed_microsec = (res == 1 ? nom : nom / denom) * 1000000.0;
            camera_settings settings;
            if (current_settings(sockfd, settings) && settings.shutter_speed > 0 &&
                shutter_speed_microsec != shutter_speed_microseconds(settings)) {
              const shutter_speed_stop change = shutter_speed_microsec < shutter_speed_microseconds(settings) ? shutter_speed_one_stop_faster : shutter_speed_one_stop_slower;
              uint64_t last_shutter_speed = 0;
              do {
                last_shutter_speed = shutter_speed_microseconds(settings);
                if (!update_setting(sockfd, change))
                  break;
              } while(current_settings(sockfd, settings) && 
                      shutter_speed_microseconds(settings) != last_shutter_speed && 
                      shutter_speed_microsec != shutter_speed_microseconds(settings) &&
                      change == (shutter_speed_microsec < shutter_speed_microseconds(settings) ? shutter_speed_one_stop_faster : shutter_speed_one_stop_slower));
              print(settings);
            }
          } 
        }
      } break;
      case command::white_balance: {
        if (splitLine.size() > 1) {
          int const wbvalue = std::stoi(splitLine[1], 0, 0);
          printf("%s(%d)\n", splitLine[0].c_str(), wbvalue);
          white_balance_mode wb;
          if (parse_white_balance_mode(wbvalue, wb) && update_setting(sockfd, wb)) {
            camera_settings settings;
            if (current_settings(sockfd, settings))
              print(settings);
          } else {
            printf("Failed to set white_balance %d\n", wbvalue);
          }
        }
      } break;

      case command::current_settings: {
        camera_settings settings;
        if (current_settings(sockfd, settings))
          print(settings);
        else
          printf("fail\n");
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
