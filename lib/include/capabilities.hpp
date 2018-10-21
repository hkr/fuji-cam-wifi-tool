#ifndef FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
#define FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP

#include <stdint.h>
#include <string>
#include <vector>

namespace fcwt {

const uint16_t capability_max_values = 32;

enum property_codes: uint16_t {
  property_white_balance         = 0x5005,
  property_aperture              = 0x5007,
  property_focus_mode            = 0x500a,
  property_shooting_mode         = 0x500e,
  property_flash                 = 0x500c,
  property_exposure_compensation = 0x5010,
  property_self_timer            = 0x5012,
  property_film_simulation       = 0xd001,
  property_image_format          = 0xd018,
  property_recmode_enable        = 0xd019,
  property_f_ss_control          = 0xd028,
  property_iso                   = 0xd02a,
  property_movie_iso             = 0xd02b,
  property_focus_point           = 0xd17c,
  property_focus_lock            = 0xd209,
  property_device_error          = 0xd21b,
  property_image_space_sd        = 0xd229,
  property_movie_remaining_time  = 0xd22a,
  property_shutter_speed         = 0xd240,
  property_image_aspect          = 0xd241,
  property_battery_level         = 0xd242,
  property_unknown
};

bool is_known_property(uint16_t value);
std::string to_string(property_codes property);


struct capability {
  property_codes property_code = property_unknown;
  uint16_t data_type = 0;
  uint8_t get_set = 0;
  uint32_t default_value = 0;
  uint32_t current_value = 0;
  uint8_t form_flag = 0;
  uint32_t min_value = 0;
  uint32_t max_value = 0;
  uint32_t step_size = 0;
  uint16_t count = 0;
  uint32_t values[capability_max_values] = {0};
};

void print(std::vector<capability> const& caps);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
