#ifndef FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
#define FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP

#include <stdint.h>
#include <string>

namespace fcwt {

const uint16_t capability_max_modes = 32;

struct capability {
  uint32_t min_value = 0;
  uint32_t value = 0;
  uint16_t count = 0;
  uint32_t modes[capability_max_modes] = {0};
};

struct aperture_f_number {
  uint32_t value = 0;
};

std::string to_string(aperture_f_number aperture);

struct auto_focus_caps {
  // TODO
};

struct camera_capabilities {
  auto_focus_caps auto_focus;
  capability iso;
  capability shutter;
  capability aperture;
  capability flash;
  capability white_balance;
  capability film_simulation;
  capability recording;
  capability self_timer;
  capability exposure_compensation;
};

void print(camera_capabilities const& caps);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
