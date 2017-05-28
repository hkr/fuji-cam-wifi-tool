#ifndef FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
#define FUJI_CAM_WIFI_TOOL_SETTINGS_HPP

#include <stdint.h>
#include <string>

#include "capabilities.hpp"

namespace fcwt {

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

enum film_simulation_mode {
  film_simulation_standard = 1,
  film_simulation_vivid = 2,
  film_simulation_soft = 3,
  film_simulation_monochrome = 4,
  film_simulation_sepia = 5,
  film_simulation_pro_neg_hi = 6,
  film_simulation_pro_neg_std = 7,
  film_simulation_monochrome_y_filter = 8,
  film_simulation_monochrome_r_filter = 9,
  film_simulation_monochrome_g_filter = 10,
  film_simulation_classic_chrome = 11,
  film_simulation_count
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

enum shutter_type {
  mechanical_shutter,
  electronic_shutter
};
char const* to_string(shutter_type shutter);

struct camera_settings {
  uint32_t iso;
  bool one_div_shutter_speed;
  uint32_t shutter_speed;
  white_balance_mode white_balance;
  film_simulation_mode film_simulation;
  auto_focus_point focus_point;
  image_settings image;
  aperture_f_number aperture;
  int32_t exposure;
  shutter_type shutter;
  int32_t battery_level;
};

void print(camera_settings const& settings);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
