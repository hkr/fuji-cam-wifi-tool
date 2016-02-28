#ifndef FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
#define FUJI_CAM_WIFI_TOOL_SETTINGS_HPP

#include <stdint.h>
#include <string>

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
  film_simulation_standard,
  film_simulation_vivid,
  film_simulation_soft,
  film_simulation_classic_chrome,
  film_simulation_pro_neg_hi,
  film_simulation_pro_neg_std,
  film_simulation_monochrome,
  film_simulation_monochrome_y_filter,
  film_simulation_monochrome_r_filter,
  film_simulation_monochrome_g_filter,
  film_simulation_sepia
};

char const* to_string(film_simulation_mode film_simulation);

enum white_balance_mode {
  white_balance_auto,
  white_balance_custom,
  white_balance_temperature,
  white_balance_fine,
  white_balance_shade,
  white_balance_fluorescent_1,
  white_balance_fluorescent_2,
  white_balance_fluorescent_3,
  white_balance_incandescent,
  white_balance_underwater
};

char const* to_string(white_balance_mode white_balance);

struct camera_settings {
  uint32_t iso;
  white_balance_mode white_balance;
  film_simulation_mode film_simulation;
  auto_focus_point focus_point;
  image_settings image;
};

void print(camera_settings const& settings);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
