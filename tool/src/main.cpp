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

#ifdef WITH_OPENCV
#include <opencv2/opencv.hpp>

using namespace cv;
#endif

namespace fcwt {

log_settings log_conf;

#ifdef WITH_OPENCV
void image_stream_cv_main(std::atomic<bool>& flag) {
  log(LOG_INFO, "image_stream_cv_main");
  sock const sockfd3 = connect_to_camera(jpg_stream_server_port);

  std::vector<uint8_t> buffer(1024 * 1024);

  if (sockfd3 <= 0) return;

  unsigned int image = 0;
  namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
  while (flag) {
    size_t receivedBytes =
        fuji_receive(sockfd3, buffer.data(), buffer.size());

    int const header = 14;  // not sure what's in the first 14 bytes
    Mat rawData = Mat( 1, receivedBytes, CV_8UC1, &buffer[header]);
    Mat decodedImage  =  imdecode( rawData , CV_LOAD_IMAGE_COLOR);

    if ( decodedImage.data == NULL )
    {
        log(LOG_WARN, "couldn't decode image");
    }
    imshow( "Display window", decodedImage );

    waitKey(25);
  }
}
#endif

void image_stream_main(std::atomic<bool>& flag) {
  log(LOG_INFO, "image_stream_main");
  sock const sockfd3 = connect_to_camera(jpg_stream_server_port);

  std::vector<uint8_t> buffer(1024 * 1024);

  if (sockfd3 <= 0) return;

  unsigned int image = 0;
  while (flag) {
    size_t receivedBytes =
        fuji_receive(sockfd3, buffer.data(), buffer.size());
    log(LOG_DEBUG, string_format("image_stream_main received %zd bytes", receivedBytes));

    char filename[1024];
    snprintf(filename, sizeof(filename), "out/img_%d.jpg", image++);
    FILE* file = fopen(filename, "wb");
    if (file) {
      int const header = 14;  // not sure what's in the first 14 bytes
      fwrite(&buffer[header], receivedBytes, 1, file);
      fclose(file);
    } else {
      log(LOG_WARN, string_format("image_stream_main Failed to create file %s", filename));
    }
  }
}

char const* commandStrings[] = {"connect", "shutter", "stream",
                                "info", "set_iso", "set_aperture", "aperture",
                                "shutter_speed", "set_shutter_speed",
                                "white_balance", "current_settings",
#ifdef WITH_OPENCV
                                "stream_cv",
#endif
};

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
#ifdef WITH_OPENCV
  stream_cv,
#endif
  unknown,
  count = unknown
};

static void completion(char const* buf, linenoiseCompletions* lc) {
  for (int i = 0; i < static_cast<int>(command::count); ++i) {
    char const* const cmd = commandStrings[i];
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
    if (line == commandStrings[i]) return static_cast<command>(i);
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

int main(int const argc, char const* argv[]) {
  uint8_t log_level = LOG_INFO;

  if (argc > 1) {
    std::string arg = argv[1];

    if (arg == "-l" || arg == "--log-level"){
      std::stringstream convert(argv[2]);
      convert >> log_level;
    }
  }

  log_conf.level = log_level;

  /* Set the completion callback. This will be called every time the
   * user uses the <tab> key. */
  linenoiseSetCompletionCallback(completion);

  sock sockfd;
  sock sockfd2;
  std::atomic<bool> imageStreamFlag(true);
  std::thread imageStreamThread;
#ifdef WITH_OPENCV
  std::thread imageStreamCVThread;
#endif
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
            log(LOG_ERROR, "failure\n");
          else {
            log(LOG_INFO, "Received camera capabilities");
            print(caps);
            camera_settings settings;
            if (current_settings(sockfd, settings))
              log(LOG_INFO, "Received camera settings");
              print(settings);
            sockfd2 = connect_to_camera(async_response_server_port);
          }
        } else {
          log(LOG_INFO, "already connected\n");
        }
      } break;
      case command::shutter: {
        if (!shutter(sockfd, sockfd2)) log(LOG_ERROR, "failure\n");

      } break;

      case command::stream: {
        imageStreamThread =
            std::thread(([&]() { image_stream_main(imageStreamFlag); }));
      } break;

#ifdef WITH_OPENCV
      case command::stream_cv: {
        imageStreamCVThread =
            std::thread(([&]() { image_stream_cv_main(imageStreamFlag); }));
      } break;
#endif

      case command::info: {
        camera_settings settings;
        if (current_settings(sockfd, settings))
          print(settings);
      } break;

      case command::set_iso: {
        if (splitLine.size() > 1) {
          unsigned long iso = std::stoul(splitLine[1], 0, 0);
          log(LOG_DEBUG, string_format("%s(%lu)", splitLine[0].c_str(), iso));
          if (update_setting(sockfd, iso_level(iso))) {
            camera_settings settings;
            if (current_settings(sockfd, settings))
              print(settings);
          } else {
            log(LOG_ERROR, string_format("Failed to set ISO %lu", iso));
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
          log(LOG_DEBUG, string_format("%s(%i)", splitLine[0].c_str(), aperture_stops));
          if (aperture_stops != 0) {
            if (update_setting(sockfd, aperture_stops < 0 ? aperture_open_third_stop : aperture_close_third_stop)) {
              camera_settings settings;
              if (current_settings(sockfd, settings))
                print(settings);
            } else {
              log(LOG_ERROR, string_format("Failed to adjust aperture %i", aperture_stops));
            }
          }
        }
      } break;

      case command::shutter_speed: {
        if (splitLine.size() > 1) {
          int shutter_stops = std::stoi(splitLine[1], 0, 0);
          log(LOG_DEBUG, string_format("%s(%i)", splitLine[0].c_str(), shutter_stops));
          if (shutter_stops != 0) {
            if (update_setting(sockfd, shutter_stops < 0 ? shutter_speed_one_stop_slower : shutter_speed_one_stop_faster)) {
              camera_settings settings;
              if (current_settings(sockfd, settings))
                print(settings);
            } else {
              log(LOG_ERROR, string_format("Failed to adjust shutter speed %i", shutter_stops));
            }
          }
        }
      } break;

      case command::set_shutter_speed: {
        if (splitLine.size() > 1) {
          double nom, denom;
          int res = std::sscanf(splitLine[1].c_str(), "%lf/%lf", &nom, &denom);
          if (res > 0) {
            const uint64_t new_speed = (res == 1 ? nom : nom / denom) * 1000000.0;
            camera_settings settings;
            if (current_settings(sockfd, settings) && settings.shutter.speed > 0 &&
                new_speed != ss_to_microsec(settings.shutter.speed)) {
              const shutter_speed_stop change = new_speed < ss_to_microsec(settings.shutter.speed) ? shutter_speed_one_stop_faster : shutter_speed_one_stop_slower;
              uint64_t last_speed = 0;
              do {
                last_speed = ss_to_microsec(settings.shutter.speed);
                if (!update_setting(sockfd, change))
                  break;
              } while(current_settings(sockfd, settings) &&
                      ss_to_microsec(settings.shutter.speed) != last_speed &&
                      new_speed != ss_to_microsec(settings.shutter.speed) &&
                      change == (new_speed < ss_to_microsec(settings.shutter.speed) ? shutter_speed_one_stop_faster : shutter_speed_one_stop_slower));
              print(settings);
            }
          }
        }
      } break;

      case command::white_balance: {
        if (splitLine.size() > 1) {
          int const wbvalue = std::stoi(splitLine[1], 0, 0);
          log(LOG_DEBUG, string_format("%s(%d)", splitLine[0].c_str(), wbvalue));
          white_balance_mode wb;
          if (parse_white_balance_mode(wbvalue, wb) && update_setting(sockfd, wb)) {
            camera_settings settings;
            if (current_settings(sockfd, settings))
              print(settings);
          } else {
            log(LOG_ERROR, string_format("Failed to set white_balance %d", wbvalue));
          }
        }
      } break;

      case command::current_settings: {
        camera_settings settings;
        if (current_settings(sockfd, settings))
          print(settings);
        else
          log(LOG_ERROR, "fail");
      } break;

      default: { log(LOG_ERROR, string_format("Unreconized command: %s", line.c_str())); }
    }
  }

  if (imageStreamThread.joinable()) {
    imageStreamFlag = false;
    imageStreamThread.join();
  }

#ifdef WITH_OPENCV
  if (imageStreamCVThread.joinable()) {
    imageStreamFlag = false;
    imageStreamCVThread.join();
  }
#endif

  terminate_control_connection(sockfd);

  return 0;
}

}  // namespace fcwt

int main(const int argc, char const* argv[]) { return fcwt::main(argc, argv); }
