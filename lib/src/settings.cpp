#include "settings.hpp"

#include "capabilities.hpp"
#include "log.hpp"

#include <stdio.h>

namespace fcwt {

#define CASE_RETURN_ENUM_TO_STRING(x) \
  case x:                             \
    return #x

char const* to_string(image_format const format) {
  switch (format) {
    CASE_RETURN_ENUM_TO_STRING(image_format_jpeg);
    CASE_RETURN_ENUM_TO_STRING(image_format_raw_and_jpeg);
  }
  FCWT_UNREACHABLE;
}

char const* to_string(jpeg_quality const quality) {
  switch (quality) {
    CASE_RETURN_ENUM_TO_STRING(jpeg_quality_fine);
    CASE_RETURN_ENUM_TO_STRING(jpeg_quality_normal);
  }
  FCWT_UNREACHABLE;
}

char const* to_string(jpeg_size const size) {
  switch (size) {
    CASE_RETURN_ENUM_TO_STRING(jpeg_size_s);
    CASE_RETURN_ENUM_TO_STRING(jpeg_size_m);
    CASE_RETURN_ENUM_TO_STRING(jpeg_size_l);
  }
  FCWT_UNREACHABLE;
}

char const* to_string(jpeg_aspect const aspect) {
  switch (aspect) {
    CASE_RETURN_ENUM_TO_STRING(jpeg_aspect_3_by_2);
    CASE_RETURN_ENUM_TO_STRING(jpeg_aspect_16_by_9);
    CASE_RETURN_ENUM_TO_STRING(jpeg_aspect_1_by_1);
  }
  FCWT_UNREACHABLE;
}

char const* to_string(film_simulation_mode const film_simulation) {
  switch (film_simulation) {
    CASE_RETURN_ENUM_TO_STRING(film_simulation_standard);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_vivid);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_soft);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_classic_chrome);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_pro_neg_hi);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_pro_neg_std);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome_y_filter);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome_r_filter);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome_g_filter);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_sepia);
  }
  FCWT_UNREACHABLE;
}

char const* to_string(white_balance_mode const white_balance) {
  switch (white_balance) {
    CASE_RETURN_ENUM_TO_STRING(white_balance_auto);
    CASE_RETURN_ENUM_TO_STRING(white_balance_custom);
    CASE_RETURN_ENUM_TO_STRING(white_balance_temperature);
    CASE_RETURN_ENUM_TO_STRING(white_balance_fine);
    CASE_RETURN_ENUM_TO_STRING(white_balance_shade);
    CASE_RETURN_ENUM_TO_STRING(white_balance_fluorescent_1);
    CASE_RETURN_ENUM_TO_STRING(white_balance_fluorescent_2);
    CASE_RETURN_ENUM_TO_STRING(white_balance_fluorescent_3);
    CASE_RETURN_ENUM_TO_STRING(white_balance_incandescent);
    CASE_RETURN_ENUM_TO_STRING(white_balance_underwater);
  }
  FCWT_UNREACHABLE;
}

#define CASE_ASSIGN_AND_BREAK(r, x) case x: r = x; break

bool parse_white_balance_mode(uint16_t const value, white_balance_mode& mode) {
  switch (value) {
  default:
    return false;
    CASE_ASSIGN_AND_BREAK(mode, white_balance_auto);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_fine);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_custom);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_temperature);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_shade);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_fluorescent_1);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_fluorescent_2);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_fluorescent_3);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_incandescent);
    CASE_ASSIGN_AND_BREAK(mode, white_balance_underwater);
  }
  return true;
}

std::string to_string(auto_focus_point const& focus_point) {
  return string_format("auto_focus_point: (%d, %d)",
                       static_cast<int>(focus_point.x),
                       static_cast<int>(focus_point.y));
}

std::string to_string(image_settings const& image) {
  return string_format(
      "image_settings: image_format=%s, jpeg_quality=%s, jpeg_size=%s, "
      "jpeg_aspect=%s, space_on_sdcard=%u",
      to_string(image.format), to_string(image.quality), to_string(image.size),
      to_string(image.aspect),
      static_cast<unsigned int>(image.space_on_sdcard));
}

void print(camera_settings const& settings) {
  printf("camera settings:\n");
  iso_level iso = {settings.iso};
  printf("\tiso: %s\n", to_string(iso).c_str());
  printf("\tshutter_speed: %s%.1fs\n", settings.one_div_shutter_speed ? "1/" : "", static_cast<double>(settings.shutter_speed) / 1000.0);
  printf("\taperture: %s\n", to_string(settings.aperture).c_str());
  printf("\twhite_balance: %s\n", to_string(settings.white_balance));
  printf("\tfilm_simulation_mode: %s\n", to_string(settings.film_simulation));
  printf("\t%s\n", to_string(settings.focus_point).c_str());
  printf("\t%s\n", to_string(settings.image).c_str());
}

}  // namespace fcwt
