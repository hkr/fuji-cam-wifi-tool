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

char const* usage =
  "Usage:\n"
  "    help|connect|info|shutter|focus-unlock\n"
  "    increment|decrement TARGET\n"
  "    set TARGET VALUE1 [VALUE2]\n"
  "    stream [cv]\n"
  "TARGET:\n"
  "    iso:           takes an integer value (e.g. 200)\n"
  "    f-number:      takes a float value (e.g. 3.5)\n"
  "    focus-point:   takes a pair of values denoting the focus point coordinates X and Y (e.g. 5 5)\n"
  "    shutter-speed: takes a value of the form N/M or N (e.g. 1/20, 3)\n"
  "    exposure-compensation:\n"
  "                   takes a float value (e.g. 1.3)\n"
  "    white-balance|film-simulation|self-timer|flash:\n"
  "                   takes an integer value. Value is one of the values in camera properties\n";

char const* completion_strings[] = {
  "connect",
  "info",
  "help",
  "shutter",
  "increment f-number",
  "increment shutter-speed",
  "increment exposure-compensation",
  "decrement f-number",
  "decrement shutter-speed",
  "decrement exposure-compensation",
  "set iso",
  "set f-number",
  "set shutter-speed",
  "set exposure-compensation",
  "set white-balance",
  "set film-simulation",
  "set self-timer",
  "set flash",
  "set focus-point",
  "focus-unlock",
  "stream",
#ifdef WITH_OPENCV
  "stream cv",
#endif
  NULL
};

char const* commandStrings[] = {
  "connect", "info", "help", "shutter", "increment",
  "decrement", "set", "focus-unlock", "stream", NULL
};

enum class command {
  connect,
  info,
  help,
  shutter,
  increment,
  decrement,
  set,
  focus_unlock,
  stream,
  unknown,
  count = unknown
};

static void completion(char const* buf, linenoiseCompletions* lc) {
  char const* pos = strchr(buf, ' ');
  if (pos == NULL) {
    for (int i = 0; commandStrings[i] != NULL; ++i) {
      if (strncmp(buf, commandStrings[i], strlen(buf)) == 0)
        linenoiseAddCompletion(lc, commandStrings[i]);
    }
  } else {
    for (int i = 0; completion_strings[i] != NULL; ++i) {
      if (strncmp(buf, completion_strings[i], strlen(buf)) == 0)
        linenoiseAddCompletion(lc, completion_strings[i]);
    }
  }
}

bool getline(std::string& line) {
  char* const str = linenoise("fcwt> ");
  if (!str) return false;

  line.assign(str);
  free(str);
  return true;
}

command parse_command(std::string const& cmd) {
  for (int i = 0; i < static_cast<int>(command::count); ++i) {
    if (cmd == commandStrings[i]) return static_cast<command>(i);
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
    if (arg == "-l" || arg == "--log-level")
      log_level = std::stoi(argv[2], 0, 0);
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
  std::vector<capability> caps;
  current_properties settings;

  std::string line;
  while (getline(line)) {
    linenoiseHistoryAdd(line.c_str());
    auto const args = split(line);
    if (args.empty()) continue;

    bool decrement = false;
    bool success = false;

    command cmd = parse_command(args[0]);
    switch (cmd) {
      case command::help: {
        printf(usage);
      } break;

      case command::connect: {
        if (sockfd <= 0) {
          sockfd = connect_to_camera(control_server_port);
          if (!init_control_connection(sockfd, "HackedClient", &caps)){
            log(LOG_ERROR, "Failed to initialize control channel");
          } else {
            log(LOG_INFO, "Received camera capabilities");
            print(caps);
            if (current_settings(sockfd, settings)) {
              log(LOG_INFO, "Received camera settings");
              print(settings);
            }
            sockfd2 = connect_to_camera(async_response_server_port);
          }
        } else {
          log(LOG_INFO, "already connected");
        }
      } break;

      case command::info: {
        if (current_settings(sockfd, settings))
          print(settings);
        else
          log(LOG_ERROR, "Failed to fetch camera settings");

      } break;

      case command::shutter: {
        if (!shutter(sockfd, sockfd2))
          log(LOG_ERROR, "Failed to release shutter");

      } break;

      case command::stream: {
        if (args.size() == 1)
          imageStreamThread =
              std::thread(([&]() { image_stream_main(imageStreamFlag); }));

#ifdef WITH_OPENCV
        else if (args[1].compare("cv") == 0)
          imageStreamCVThread =
              std::thread(([&]() { image_stream_cv_main(imageStreamFlag); }));
#endif
      } break;

      case command::set: {
        if (args.size() < 3) {
          printf(usage);
          break;
        }

        if (args[1].compare("iso") == 0) {
          unsigned long value = std::stoul(args[2], 0, 0);
          success = update_setting(sockfd, property_iso, value);

        } else if (args[1].compare("white-balance") == 0) {
          uint32_t const value = std::stoi(args[2], 0, 0);
          if (is_known_property_value(property_white_balance, value))
            success = update_setting(sockfd, property_white_balance, value);

        } else if (args[1].compare("film-simulation") == 0) {
          uint32_t const value = std::stoi(args[2], 0, 0);
          if (is_known_property_value(property_film_simulation, value))
            success = update_setting(sockfd, property_film_simulation, value);

        } else if (args[1].compare("flash") == 0) {
          uint32_t const value = std::stoi(args[2], 0, 0);
          if (is_known_property_value(property_flash, value))
            success = update_setting(sockfd, property_flash, value);

        } else if (args[1].compare("self-timer") == 0) {
          uint32_t const value = std::stoi(args[2], 0, 0);
          if (is_known_property_value(property_self_timer, value))
            success = update_setting(sockfd, property_self_timer, value);

        } else if (args[1].compare("f-number") == 0) {
          uint32_t const value = static_cast<uint32_t>(std::stod(args[2]) * 100.0);
          if (current_settings(sockfd, settings) && settings.values[property_aperture] > 0 &&
              value != settings.values[property_aperture]) {
            const fnumber_update_direction direction = value < settings.values[property_aperture] ? fnumber_decrement : fnumber_increment;
            uint32_t last_value = 0;

            do {
              last_value = settings.values[property_aperture];
              success = update_setting(sockfd, direction);
              if (!success)
                break;
            } while(current_settings(sockfd, settings) && 
                    settings.values[property_aperture] != last_value && 
                    value != settings.values[property_aperture] &&
                    direction == (value < settings.values[property_aperture] ? fnumber_decrement : fnumber_increment));
          } 

        } else if (args[1].compare("shutter-speed") == 0) {
          double nom, denom;
          int res = std::sscanf(args[2].c_str(), "%lf/%lf", &nom, &denom);
          if (res <= 0) {
            log(LOG_INFO, "Could not parse provided value");
            break;
          }
          double value = (res == 1 ? nom : nom / denom) * 1000000.0;
          if (current_settings(sockfd, settings) && settings.values[property_shutter_speed] > 0 &&
              value != ss_to_microsec(settings.values[property_shutter_speed])) {
            const ss_update_direction direction = value < ss_to_microsec(settings.values[property_shutter_speed]) ? ss_increment : ss_decrement;
            uint64_t last_speed = 0;
            do {
              last_speed = ss_to_microsec(settings.values[property_shutter_speed]);
              success = update_setting(sockfd, direction);
              if (!success)
                break;
            } while(current_settings(sockfd, settings) &&
                    ss_to_microsec(settings.values[property_shutter_speed]) != last_speed &&
                    ss_to_microsec(settings.values[property_shutter_speed]) != value &&
                    direction == (value < ss_to_microsec(settings.values[property_shutter_speed]) ? ss_increment : ss_decrement));
          }

        } else if (args[1].compare("exposure-compensation") == 0) {
          uint32_t const value = static_cast<uint32_t>(std::stod(args[2]) * 1000.0);
          if (current_settings(sockfd, settings) && value != settings.values[property_exposure_compensation]) {
            const exp_update_direction direction = value < settings.values[property_exposure_compensation] ? exp_decrement : exp_increment;
            uint32_t last_value = 0;
            do {
              last_value = settings.values[property_exposure_compensation];
              success = update_setting(sockfd, direction);
              if (!success)
                break;
            } while(current_settings(sockfd, settings) && 
                    settings.values[property_exposure_compensation] != last_value && 
                    value != settings.values[property_exposure_compensation] &&
                    direction == (value < settings.values[property_exposure_compensation] ? exp_decrement : exp_increment));
          } 

        } else if (args[1].compare("focus-point") == 0 && args.size() == 4) {
          auto_focus_point point = 0;
          point.x = std::stoi(args[2], 0, 0);
          point.y = std::stoi(args[3], 0, 0);
          if (point.x * point.y <= 0) {
            log(LOG_INFO, "Could not parse provided point");
            break;
          }

          success = update_setting(sockfd, point);

        } else {
            printf(usage);
            break;
        }

        if (success) {
          if (current_settings(sockfd, settings))
            print(settings);
        } else {
          log(LOG_ERROR, string_format("Failed to set %s to %s", args[1].c_str(), args[2].c_str()));
        }
      } break;

      case command::decrement:
        decrement = true;
      case command::increment: {
        if (args.size() != 2) {
          printf(usage);
          break;
        }

        if (args[1].compare("f-number") == 0)
          success = update_setting(sockfd, decrement ? fnumber_decrement : fnumber_increment);

        else if (args[1].compare("shutter-speed") == 0)
          success = update_setting(sockfd, decrement ? ss_decrement : ss_increment);

        else if (args[1].compare("exposure-compensation") == 0)
          success = update_setting(sockfd, decrement ? exp_decrement : exp_increment);

        else {
            printf(usage);
            break;
        }

        if (success) {
          if (current_settings(sockfd, settings))
            print(settings);
        } else {
          log(LOG_ERROR, string_format("Failed to %s %s", decrement ? "decrement":"increment", args[1].c_str()));
        }
      } break;

      case command::focus_unlock: {
        success = unlock_focus(sockfd);

        if (success) {
          if (current_settings(sockfd, settings))
            print(settings);
        } else {
          log(LOG_ERROR, string_format("Failed to unlock camera focus"));
        }
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
