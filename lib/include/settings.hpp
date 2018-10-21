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

// when querying settings from camera we get RAW + FINE even when it's in pure RAW mode
// so no extra image_format_raw
#define IMAGE_FORMAT_FINE 2
#define IMAGE_FORMAT_NORMAL 3
#define IMAGE_FORMAT_FINE_RAW  4
#define IMAGE_FORMAT_NORMAL_RAW  5
#define IMAGE_ASPECT_S_3x2  2
#define IMAGE_ASPECT_S_16x9  3
#define IMAGE_ASPECT_S_1x1  4
#define IMAGE_ASPECT_M_3x2  6
#define IMAGE_ASPECT_M_16x9  7
#define IMAGE_ASPECT_M_1x1  8
#define IMAGE_ASPECT_L_3x2  10
#define IMAGE_ASPECT_L_16x9  11
#define IMAGE_ASPECT_L_1x1  12
#define FOCUS_MANUAL  1
#define FOCUS_SINGLE_AUTO  32769
#define FOCUS_CONTINUOUS_AUTO  32770
#define TIMER_OFF  0
#define TIMER_1SEC  1
#define TIMER_2SEC  2
#define TIMER_5SEC  3
#define TIMER_10SEC  4
#define FILM_SIMULATION_PROVIA  1
#define FILM_SIMULATION_VELVIA  2
#define FILM_SIMULATION_ASTIA  3
#define FILM_SIMULATION_MONOCHROME  4
#define FILM_SIMULATION_SEPIA  5
#define FILM_SIMULATION_PRO_NEG_HI  6
#define FILM_SIMULATION_PRO_NEG_STD  7
#define FILM_SIMULATION_MONOCHROME_Y_FILTER  8
#define FILM_SIMULATION_MONOCHROME_R_FILTER  9
#define FILM_SIMULATION_MONOCHROME_G_FILTER  10
#define FILM_SIMULATION_CLASSIC_CHROME  11
#define FILM_SIMULATION_ACROS  12
#define FILM_SIMULATION_ACROS_Y  13
#define FILM_SIMULATION_ACROS_R  14
#define FILM_SIMULATION_ACROS_G  15
#define FILM_SIMULATION_ETERNA  16
#define WHITE_BALANCE_AUTO  2
#define WHITE_BALANCE_FINE  4
#define WHITE_BALANCE_INCANDESCENT  6
#define WHITE_BALANCE_FLUORESCENT_1  0X8001
#define WHITE_BALANCE_FLUORESCENT_2  0X8002
#define WHITE_BALANCE_FLUORESCENT_3  0X8003
#define WHITE_BALANCE_SHADE  0X8006
#define WHITE_BALANCE_UNDERWATER  0X800a
#define WHITE_BALANCE_TEMPERATURE  0X800b
#define WHITE_BALANCE_CUSTOM  0X800c 
#define MOVIE_BUTTON_UNAVAILABLE  0
#define MOVIE_BUTTON_AVAILABLE  1
#define SHOOTING_MANUAL  1
#define SHOOTING_PROGRAM  2
#define SHOOTING_APERTURE_PRIORITY  3
#define SHOOTING_SHUTTER_PRIORITY  4
#define SHOOTING_AUTO  6
#define BATTERY_CRITICAL  1
#define BATTERY_ONE_BAR  2
#define BATTERY_TWO_BAR  3
#define BATTERY_FULL  4
#define FLASH_AUTO  1
#define FLASH_OFF  2
#define FLASH_FILL  3
#define FLASH_REDEYE_AUTO  4
#define FLASH_REDEYE_FILL  5
#define FLASH_EXTERNAL_SYNC  6
#define FLASH_ON  32769
#define FLASH_REDEYE  32770
#define FLASH_REDEYE_ON  32771
#define FLASH_REDEYE_SYNC  32772
#define FLASH_REDEYE_REAR  32773
#define FLASH_SLOW_SYNC  32774
#define FLASH_REAR_SYNC  32775
#define FLASH_COMMANDER  32776
#define FLASH_DISABLE  32777
#define FLASH_ENABLE  32778
#define F_SS_CTRL_BOTH  0
#define F_SS_CTRL_F  1
#define F_SS_CTRL_SS  2
#define F_SS_CTRL_NONE  3
#define FOCUS_LOCK_OFF  0
#define FOCUS_LOCK_ON  1
#define DEVICE_ERROR_NONE 0

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
char const* to_string(shutter_speed speed);

struct current_properties {
  std::vector<property_codes> camera_order;
  std::map<property_codes, uint32_t> values;
};

void print(current_properties& settings);

double ss_to_microsec(uint32_t raw_speed);
}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
