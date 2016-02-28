#ifndef FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
#define FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP

#include <stdint.h>
#include <string>

namespace fcwt {

const uint32_t iso_flag_auto = 1 << 31;
const uint32_t iso_flag_emulated = 1 << 30;
const uint32_t iso_value_mask = 0x00ffffff;
const uint32_t iso_max_levels = 32;

struct iso_level {
  iso_level(uint32_t val) : value(val) {}
  operator uint32_t() const { return value; }
  uint32_t value;
};

std::string to_string(iso_level iso);

struct iso_caps {
  uint32_t levels[iso_max_levels] = {};
  uint32_t numLevels = 0;
};

enum shutter_speed_mode { shutter_speed_auto, shutter_speed_manual };

struct shutter_speed_caps {
  shutter_speed_mode mode = shutter_speed_auto;
  uint32_t value = 0;
  int32_t exposure = 0;
};

struct aperture_caps {
  uint32_t value = 0;
};

struct auto_focus_caps {
  // TODO
};

struct camera_capabilities {
  iso_caps iso;
  shutter_speed_caps shutter_speed;
  auto_focus_caps auto_focus;
  aperture_caps aperture;
};

void print(camera_capabilities const& caps);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
