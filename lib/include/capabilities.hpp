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

enum data_type : uint16_t {
	data_type_unknown = 0,
	data_type_int8 = 1,
	data_type_uint8 = 2,
	data_type_int16 = 3,
	data_type_uint16 = 4,
	data_type_int32 = 5,
	data_type_uint32 = 6
};

inline bool is_signed(data_type dt)
{
	switch (dt) {
	default:
		return false;
	case data_type_int8:
	case data_type_int16:
	case data_type_int32:
		return true;
	}
}

inline size_t data_type_size(data_type dt)
{
	switch (dt) {
	case data_type_int8:
	case data_type_uint8:
		return 1;
	case data_type_int16:
	case data_type_uint16:
		return 2;
	case data_type_int32:
	case data_type_uint32:
		return 4;
	}
	return 0;
}

struct capability {
  property_codes property_code = property_unknown;
  data_type data_type = data_type_unknown;
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
