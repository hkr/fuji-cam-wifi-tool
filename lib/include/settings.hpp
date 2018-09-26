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

enum image_format {
  image_format_jpeg,
  image_format_raw_and_jpeg
  // when querying settings from camera we get RAW + FINE even when it's in pure RAW mode
  // so no extra image_format_raw
};

enum jpeg_quality {
  jpeg_quality_fine,
  jpeg_quality_normal,
};

enum jpeg_size {
  jpeg_size_s,
  jpeg_size_m,
  jpeg_size_l,
};

enum jpeg_aspect {
  jpeg_aspect_3_by_2,
  jpeg_aspect_16_by_9,
  jpeg_aspect_1_by_1
};

struct image_settings {
  image_format format;
  jpeg_quality quality;
  jpeg_size size;
  jpeg_aspect aspect;
  uint32_t space_on_sdcard;
};
std::string to_string(image_settings const& image);

struct auto_focus_point {
  uint8_t x;
  uint8_t y;
};
std::string to_string(auto_focus_point const& focus_point);

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

enum shutter_type {
  mechanical_shutter,
  electronic_shutter
};
char const* to_string(shutter_type shutter);

struct shutter_settings {
  shutter_speed speed { 0 };
  shutter_type type;
};

enum focus_mode {
  manual_focus = 1,
  single_autofocus = 32769,
  continuous_autofocus = 32770
};
char const* to_string(focus_mode mode);

enum timer_mode {
  timer_off = 0,
  timer_1sec = 1,
  timer_2sec = 2,
  timer_5sec = 3,
  timer_10sec = 4,
};
char const* to_string(timer_mode mode);

enum film_simulation_mode {
  film_simulation_provia = 1,
  film_simulation_velvia = 2,
  film_simulation_astia = 3,
  film_simulation_monochrome = 4,
  film_simulation_sepia = 5,
  film_simulation_pro_neg_hi = 6,
  film_simulation_pro_neg_std = 7,
  film_simulation_monochrome_y_filter = 8,
  film_simulation_monochrome_r_filter = 9,
  film_simulation_monochrome_g_filter = 10,
  film_simulation_classic_chrome = 11,
  film_simulation_acros = 12,
  film_simulation_acros_y = 13,
  film_simulation_acros_r = 14,
  film_simulation_acros_g = 15,
  film_simulation_eterna = 16
};

char const* to_string(film_simulation_mode film_simulation);

enum white_balance_mode {
  white_balance_auto = 2,
  white_balance_fine = 4,
  white_balance_incandescent = 6,
  white_balance_fluorescent_1 = 0x8001,
  white_balance_fluorescent_2 = 0x8002,
  white_balance_fluorescent_3 = 0x8003,
  white_balance_shade = 0x8006,
  white_balance_underwater = 0x800A,
  white_balance_temperature = 0x800B,
  white_balance_custom = 0x800C 
};

char const* to_string(white_balance_mode white_balance);
bool parse_white_balance_mode(uint16_t const value, white_balance_mode& mode);

enum recording_mode {
  movie_button_unavailable = 0,
  movie_button_available = 1
};
char const* to_string(recording_mode mode);

enum shooting_mode {
  manual_mode = 1,
  program_mode = 2,
  aperture_priority_mode = 3,
  shutter_priority_mode = 4,
  auto_mode = 6
};
char const* to_string(recording_mode mode);

enum battery_level {
  critical_battery = 1,
  one_bar_battery = 2,
  two_bar_battery = 3,
  full_battery = 4
};
char const* to_string(battery_level level);

enum flash_mode {
  flash_auto = 1,
  flash_off = 2,
  flash_fill = 3,
  flash_redeye_auto = 4,
  flash_redeye_fill = 5,
  flash_external_sync = 6,
  flash_on = 32769,
  flash_redeye = 32770,
  flash_redeye_on = 32771,
  flash_redeye_sync = 32772,
  flash_redeye_rear = 32773,
  flash_slow_sync = 32774,
  flash_rear_sync = 32775,
  flash_commander = 32776,
  flash_disable = 32777,
  flash_enable = 32778 
};
char const* to_string(flash_mode flash);

struct camera_settings {
  uint32_t iso;
  uint32_t movie_iso;
  uint32_t device_error;
  uint32_t movie_hd_remaining_time;
  int32_t exposure_compensation;
  white_balance_mode white_balance;
  film_simulation_mode film_simulation;
  auto_focus_point focus_point;
  image_settings image;
  aperture_f_number aperture;
  shutter_settings shutter;
  battery_level battery;
  flash_mode flash;
  timer_mode self_timer;
  focus_mode focus;
  recording_mode recording;
  shooting_mode shooting;
};

void print(camera_settings const& settings);

double ss_to_microsec(uint32_t raw_speed);
}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
