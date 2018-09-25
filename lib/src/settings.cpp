#include "settings.hpp"

#include "capabilities.hpp"
#include "log.hpp"

#include <stdio.h>

namespace fcwt {

#define CASE_RETURN_ENUM_TO_STRING(x) case x: return #x;
#define CASE_ASSIGN_AND_BREAK(r, x) case x: r = x; break

std::string to_string(iso_level iso) {
    auto const lvl = iso.value;
    char const* flag = "";

    // movie iso auto is  0xffffffff
    if (lvl == 0xffffffff)
        return "auto";

    if (lvl & iso_flag_auto) 
        flag = "(auto)";
    else if (lvl & iso_flag_emulated)
        flag = "(emulated)";

    return string_format("%12d %s", lvl & iso_value_mask, flag);
}

char const* to_string(shutter_speed speed) {
    auto const spd = speed.value;

    double out = static_cast<double>(spd & shutter_value_mask) / 1000.0;
    if (spd & shutter_flag_subsecond) 
        return string_format("1/%.1fs", out).c_str();
    else
        return string_format("%.1fs", out).c_str();
}

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
    CASE_RETURN_ENUM_TO_STRING(film_simulation_provia);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_velvia);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_astia);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_sepia);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_pro_neg_hi);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_pro_neg_std);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome_y_filter);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome_r_filter);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_monochrome_g_filter);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_classic_chrome);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_acros);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_acros_y);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_acros_r);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_acros_g);
    CASE_RETURN_ENUM_TO_STRING(film_simulation_eterna);
  }
  FCWT_UNREACHABLE;
}

char const* to_string(shutter_type shutter)
{
  switch (shutter) {
    CASE_RETURN_ENUM_TO_STRING(mechanical_shutter);
    CASE_RETURN_ENUM_TO_STRING(electronic_shutter);
  }
  FCWT_UNREACHABLE; 
}

char const* to_string(recording_mode mode)
{
  switch (mode) {
    CASE_RETURN_ENUM_TO_STRING(movie_button_unavailable);
    CASE_RETURN_ENUM_TO_STRING(movie_button_available);
  }
  FCWT_UNREACHABLE; 
}

char const* to_string(shooting_mode mode)
{
  switch (mode) {
    CASE_RETURN_ENUM_TO_STRING(manual_mode);
    CASE_RETURN_ENUM_TO_STRING(program_mode);
    CASE_RETURN_ENUM_TO_STRING(aperture_priority_mode);
    CASE_RETURN_ENUM_TO_STRING(shutter_priority_mode);
    CASE_RETURN_ENUM_TO_STRING(auto_mode);
  }
  FCWT_UNREACHABLE; 
}

char const* to_string(battery_level level)
{
  switch (level) {
    CASE_RETURN_ENUM_TO_STRING(critical_battery);
    CASE_RETURN_ENUM_TO_STRING(one_bar_battery);
    CASE_RETURN_ENUM_TO_STRING(two_bar_battery);
    CASE_RETURN_ENUM_TO_STRING(full_battery);
  }
  FCWT_UNREACHABLE; 
}

char const* to_string(focus_mode mode)
{
  switch (mode) {
    CASE_RETURN_ENUM_TO_STRING(manual_focus);
    CASE_RETURN_ENUM_TO_STRING(single_autofocus);
    CASE_RETURN_ENUM_TO_STRING(continuous_autofocus);
  }
  FCWT_UNREACHABLE; 
}

char const* to_string(timer_mode mode)
{
  switch (mode) {
    CASE_RETURN_ENUM_TO_STRING(timer_off);
    CASE_RETURN_ENUM_TO_STRING(timer_1sec);
    CASE_RETURN_ENUM_TO_STRING(timer_2sec);
    CASE_RETURN_ENUM_TO_STRING(timer_5sec);
    CASE_RETURN_ENUM_TO_STRING(timer_10sec);
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

char const* to_string(flash_mode const flash) {
  switch (flash) {
    CASE_RETURN_ENUM_TO_STRING(flash_auto);
    CASE_RETURN_ENUM_TO_STRING(flash_off);
    CASE_RETURN_ENUM_TO_STRING(flash_redeye_auto);
    CASE_RETURN_ENUM_TO_STRING(flash_redeye_fill);
    CASE_RETURN_ENUM_TO_STRING(flash_external_sync);
    CASE_RETURN_ENUM_TO_STRING(flash_on);
    CASE_RETURN_ENUM_TO_STRING(flash_redeye);
    CASE_RETURN_ENUM_TO_STRING(flash_redeye_on);
    CASE_RETURN_ENUM_TO_STRING(flash_redeye_sync);
    CASE_RETURN_ENUM_TO_STRING(flash_redeye_rear);
    CASE_RETURN_ENUM_TO_STRING(flash_slow_sync);
    CASE_RETURN_ENUM_TO_STRING(flash_rear_sync);
    CASE_RETURN_ENUM_TO_STRING(flash_commander);
    CASE_RETURN_ENUM_TO_STRING(flash_disable);
    CASE_RETURN_ENUM_TO_STRING(flash_enable);
  }
  FCWT_UNREACHABLE;
}

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
      "image_settings:\n"
      "\t\timage_format = %s\n"
      "\t\tjpeg_quality = %s\n"
      "\t\tjpeg_size = %s\n"
      "\t\tjpeg_aspect = %s\n"
      "\t\tspace_on_sdcard = %u",
      to_string(image.format), to_string(image.quality), to_string(image.size),
      to_string(image.aspect),
      static_cast<unsigned int>(image.space_on_sdcard));
}

void print(camera_settings const& settings) {
  iso_level iso = { settings.iso };
  iso_level movie_iso = { settings.movie_iso };
  shutter_speed speed = { settings.shutter_speed };

  printf("camera settings:\n");
  printf("\tiso: %s\n", to_string(iso).c_str());
  printf("\tshutter_speed: %s\n", to_string(speed));
  printf("\taperture: %s\n", to_string(settings.aperture).c_str());
  printf("\twhite_balance: %s\n", to_string(settings.white_balance));
  printf("\tfilm_simulation_mode: %s\n", to_string(settings.film_simulation));
  printf("\texposure_compensation: %.1f\n", static_cast<double>(settings.exposure_compensation) / 1000.0);
  printf("\tshutter_type: %s\n", to_string(settings.shutter));
  printf("\tbattery_level: %s\n", to_string(settings.battery));
  printf("\t%s\n", to_string(settings.focus_point).c_str());
  printf("\t%s\n", to_string(settings.image).c_str());
  printf("\tmovie_iso: %s\n", to_string(movie_iso).c_str());
  printf("\tflash: %s\n", to_string(settings.flash));
  printf("\tself_timer: %s\n", to_string(settings.self_timer));
  printf("\tfocus_mode: %s\n", to_string(settings.focus));
  printf("\tmovie_hd_remaining_time: %ss\n", to_string(settings.movie_hd_remaining_time).c_str());
  printf("\tshooting_mode: %s\n", to_string(settings.shooting));
  printf("\tdevice_error: %d\n", settings.device_error);
}

}  // namespace fcwt
