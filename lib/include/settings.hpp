#ifndef FUJI_CAM_WIFI_TOOL_SETTINGS_HPP
#define FUJI_CAM_WIFI_TOOL_SETTINGS_HPP

#include <stdint.h>

namespace fcwt {

enum image_format
{
  image_format_jpeg,
  image_format_raw
};

enum jpeg_quality
{
  jpeg_quality_fine,
  jpeg_quality_normal,
};

enum jpeg_size
{
  jpeg_size_s,
  jpeg_size_m,
  jpeg_size_l,
};

enum jpeg_aspect
{
  jpeg_aspect_3_by_2,
  jpeg_aspect_16_by_9,
  jpeg_aspect_1_by_1
};

struct image_settings
{
  image_format format;
  jpeg_quality quality;
  jpeg_size size;
  jpeg_aspect aspect;
};

struct auto_focus_point
{
  uint8_t x;
  uint8_t y;
};

enum film_simulation_mode
{
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

enum white_balance_mode
{
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

struct camera_settings
{
  uint32_t iso;
  white_balance_mode wb;
  film_simulation_mode film_simulation;
  auto_focus_point focus_point;
  image_settings image;
};

} // namespace fcwt

#endif // FUJI_CAM_WIFI_TOOL_SETTINGS_HPP