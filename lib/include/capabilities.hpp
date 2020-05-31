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

enum data_types : uint16_t {
	data_type_unknown = 0,
	data_type_int8 = 1,
	data_type_uint8 = 2,
	data_type_int16 = 3,
	data_type_uint16 = 4,
	data_type_int32 = 5,
	data_type_uint32 = 6
};

inline bool is_signed(data_types dt)
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

inline size_t data_type_size(data_types dt)
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
	default:
	case data_type_unknown:
		return 0;
	}
}

struct capability {
  property_codes property_code = property_unknown;
  data_types data_type = data_type_unknown;
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
#define BATTERY_126S_CRITICAL  6
#define BATTERY_126S_ONE_BAR  7
#define BATTERY_126S_TWO_BAR  8
#define BATTERY_126S_THREE_BAR  9
#define BATTERY_126S_FOUR_BAR  10
#define BATTERY_126S_FULL  11
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
}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_CAPABILITIES_HPP
