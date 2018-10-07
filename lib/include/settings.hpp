#ifndef FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
#define FUJI_CAM_WIFI_TOOL_SETTINGS_HPP

#include <stdint.h>
#include <string>

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

static std::map<property_codes, std::map<uint16_t, const char *>> property_value_strings = {
   { property_white_balance, {
        { WHITE_BALANCE_AUTO, "Auto" },
        { WHITE_BALANCE_FINE, "Fine" },
        { WHITE_BALANCE_INCANDESCENT, "Incandescent" },
        { WHITE_BALANCE_FLUORESCENT_1, "Fluorescent 1" },
        { WHITE_BALANCE_FLUORESCENT_2, "Fluorescent 2" },
        { WHITE_BALANCE_FLUORESCENT_3, "Fluorescent 3" },
        { WHITE_BALANCE_SHADE, "Shade" },
        { WHITE_BALANCE_UNDERWATER, "Underwater" },
        { WHITE_BALANCE_TEMPERATURE, "Kelvin" },
        { WHITE_BALANCE_CUSTOM, "Custom" },
   }},
   { property_focus_mode, {
        { FOCUS_MANUAL, "Manual" },
        { FOCUS_SINGLE_AUTO, "Single Autofocus"},
        { FOCUS_CONTINUOUS_AUTO, "Continuous Autofocus"},
   }},
   { property_shooting_mode, {
        { SHOOTING_MANUAL, "Manual" },
        { SHOOTING_PROGRAM, "Program" },
        { SHOOTING_APERTURE_PRIORITY, "Aperture Priority" },
        { SHOOTING_SHUTTER_PRIORITY, "Shutter Priority" },
        { SHOOTING_AUTO, "Auto" },
   }},
   { property_flash, {
        { FLASH_AUTO, "Auto" },
        { FLASH_OFF, "Off" },
        { FLASH_FILL, "Fill" },
        { FLASH_REDEYE_AUTO, "Red Eye Auto" },
        { FLASH_REDEYE_FILL, "Red Eye Fill" },
        { FLASH_EXTERNAL_SYNC, "External Sync" },
        { FLASH_ON, "On" },
        { FLASH_REDEYE, "Red Eye" },
        { FLASH_REDEYE_ON, "Red eye On" },
        { FLASH_REDEYE_SYNC, "Red Eye Sync" },
        { FLASH_REDEYE_REAR, "Red Eye Rear" },
        { FLASH_SLOW_SYNC, "Slow Sync" },
        { FLASH_REAR_SYNC, "Rear Sync" },
        { FLASH_COMMANDER, "Commander" },
        { FLASH_DISABLE, "Disabled" },
        { FLASH_ENABLE, "Enabled" },
   }},
   { property_self_timer, {
        { TIMER_OFF, "Off" },
        { TIMER_1SEC, "1 Second" },
        { TIMER_2SEC, "2 Seconds" },
        { TIMER_5SEC, "5 Seconds" },
        { TIMER_10SEC, "10 Seconds" },
   }},
   { property_film_simulation, {
        { FILM_SIMULATION_PROVIA, "Provia" },
        { FILM_SIMULATION_VELVIA, "Velvia" },
        { FILM_SIMULATION_ASTIA, "Astia" },
        { FILM_SIMULATION_MONOCHROME, "Monochrome" },
        { FILM_SIMULATION_SEPIA, "Sepia" },
        { FILM_SIMULATION_PRO_NEG_HI, "Pro-Neg Hi" },
        { FILM_SIMULATION_PRO_NEG_STD, "Pro-Neg Standard" },
        { FILM_SIMULATION_MONOCHROME_Y_FILTER, "Monochrome Y-filter" },
        { FILM_SIMULATION_MONOCHROME_R_FILTER, "Monochrome R-filter" },
        { FILM_SIMULATION_MONOCHROME_G_FILTER, "Monochrome G-filter" },
        { FILM_SIMULATION_CLASSIC_CHROME, "Classic Chrome" },
        { FILM_SIMULATION_ACROS, "Acros" },
        { FILM_SIMULATION_ACROS_Y, "Acros Y" },
        { FILM_SIMULATION_ACROS_R, "Acros R" },
        { FILM_SIMULATION_ACROS_G, "Acros G" },
        { FILM_SIMULATION_ETERNA, "Eterna" },
   }},
   { property_image_format, {
        { IMAGE_FORMAT_FINE, "JPEG Fine"},
        { IMAGE_FORMAT_NORMAL, "JPEG Normal"},
        { IMAGE_FORMAT_FINE_RAW , "RAW + JPEG Fine"},
        { IMAGE_FORMAT_NORMAL_RAW, "RAW + JPEG Normal"},
   }},
   { property_recmode_enable, {
        { MOVIE_BUTTON_UNAVAILABLE, "Unavailable" },
        { MOVIE_BUTTON_AVAILABLE, "Available" },
   }},
   { property_f_ss_control, {
        { F_SS_CTRL_BOTH, "Aperture and ShutterSpeed adjustable"},
        { F_SS_CTRL_F, "ShutterSpeed hit a min/max"},
        { F_SS_CTRL_SS, "Aperture hit a min/max"},
        { F_SS_CTRL_NONE, "ShutterSpeed and Aperture hit a min/max"},
   }},
   { property_focus_lock, {
        { FOCUS_LOCK_OFF, "Off" },
        { FOCUS_LOCK_ON, "On" },
   }},
   { property_device_error, {
        { DEVICE_ERROR_NONE, "Status OK" },
   }},
   { property_image_aspect, {
        { IMAGE_ASPECT_S_3x2, "Small 3:2"},
        { IMAGE_ASPECT_S_16x9, "Small 16:9"},
        { IMAGE_ASPECT_S_1x1, "Small 1:1"},
        { IMAGE_ASPECT_M_3x2, "Medium 3:2"},
        { IMAGE_ASPECT_M_16x9, "Medium 16:9"},
        { IMAGE_ASPECT_M_1x1, "Medium 1:1"},
        { IMAGE_ASPECT_L_3x2, "Large 3:2"},
        { IMAGE_ASPECT_L_16x9, "Large 16:9"},
        { IMAGE_ASPECT_L_1x1, "Large 1:1"},
   }},
   { property_battery_level, {
        { BATTERY_CRITICAL, "Critical" },
        { BATTERY_ONE_BAR, "One bar" },
        { BATTERY_TWO_BAR, "Two bars" },
        { BATTERY_FULL, "Full" },
   }},
};

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

void print(std::map<property_codes, uint32_t> const& settings);

double ss_to_microsec(uint32_t raw_speed);
}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
