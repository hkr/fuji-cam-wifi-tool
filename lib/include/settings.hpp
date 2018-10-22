#ifndef FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
#define FUJI_CAM_WIFI_TOOL_SETTINGS_HPP

#include <stdint.h>
#include <string>
#include <map>

#include "capabilities.hpp"

namespace fcwt {

const uint32_t iso_flag_auto = 1 << 31;
const uint32_t iso_flag_emulated = 1 << 30;
const uint32_t iso_value_mask = 0x00ffffff;
const uint32_t shutter_flag_subsecond = 1 << 31;
const uint32_t shutter_value_mask = 0x0fffffff;

std::string to_string(property_codes property, uint32_t value);
bool is_known_property_value(property_codes property, uint32_t value);

struct auto_focus_point {
  auto_focus_point(uint32_t val) : x(val>>8), y(val) {}
  uint8_t x;
  uint8_t y;
};
std::string to_string(auto_focus_point const& focus_point);

struct f_number {
  f_number(uint32_t val) : value(val) {}
  operator uint32_t() const { return value; }
  uint32_t value = 0;
};
std::string to_string(f_number aperture);

struct iso_level {
  iso_level(uint32_t val) : value(val) {}
  operator uint32_t() const { return value; }
  uint32_t value;
};
std::string to_string(iso_level iso);

struct shutter_speed {
  shutter_speed(uint32_t val) : value(val) {}
  operator uint32_t() const { return value; }
  uint32_t value;
};
std::string to_string(shutter_speed speed);

struct current_properties {
  std::vector<property_codes> camera_order;
  std::map<property_codes, uint32_t> values;
};

void print(current_properties& settings);

double ss_to_microsec(uint32_t raw_speed);
}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
