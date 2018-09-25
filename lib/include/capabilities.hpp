#ifndef FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
#define FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP

#include <stdint.h>
#include <string>

namespace fcwt {

const uint16_t capability_max_modes = 32;
const uint32_t iso_max_modes = 32;
const uint32_t iso_flag_auto = 1 << 31;
const uint32_t iso_flag_emulated = 1 << 30;
const uint32_t iso_value_mask = 0x00ffffff;
const uint32_t shutter_flag_subsecond = 1 << 31;
const uint32_t shutter_value_mask = 0x0fffffff;

struct iso_level {
  iso_level(uint32_t val) : value(val) {}
  operator uint32_t() const { return value; }
  uint32_t value;
};

std::string to_string(iso_level iso);

struct iso_levels {
  uint32_t modes[capability_max_modes] = {};
  uint32_t count = 0;
};

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
  iso_levels iso;
  auto_focus_caps auto_focus;
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
