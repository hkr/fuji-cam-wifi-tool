#include "settings.hpp"

#include "capabilities.hpp"
#include "log.hpp"

#include <stdio.h>

namespace fcwt {

const char* const unknown_value_str = "== Unknown value! ==";


const std::map<property_codes, std::map<uint16_t, const char *>> property_value_strings = {
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
		{ BATTERY_126S_CRITICAL, "Critical" },
		{ BATTERY_126S_ONE_BAR, "One bar" },
        { BATTERY_126S_TWO_BAR, "Two bars" },
        { BATTERY_126S_THREE_BAR, "Three bars" },
        { BATTERY_126S_FOUR_BAR, "Four bars" },
        { BATTERY_126S_FULL, "Full" },
   }},
   { property_unknown, {
        { 0, unknown_value_str },
   }},
};

bool is_known_property_value(property_codes property, uint32_t value)
{
    auto it = property_value_strings.find(property);
    if (it == property_value_strings.end())
        return false;

    auto itv = it->second.find(value);
    if (itv == it->second.end())
        return false;

    return true;
}

std::string to_string(property_codes property, uint32_t value)
{
    if (!is_known_property_value(property, value))
        return unknown_value_str;

    return property_value_strings.find(property)->second.find(value)->second;;
}

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

    return string_format("%6d %s", lvl & iso_value_mask, flag);
}

std::string to_string(shutter_speed speed) {
    if (static_cast<int32_t>(speed.value) == -1)
        return "----";

    bool subsecond = bool(speed & shutter_flag_subsecond);
    double out = static_cast<double>(speed & shutter_value_mask) / 1000.0;
    if (subsecond) 
        return string_format("1/%.1fs", out).c_str();
    else
        return string_format("%.1fs", out).c_str();
}

std::string to_string(auto_focus_point const& focus_point) {
  return string_format("(%d, %d)", focus_point.x, focus_point.y);
}

std::string to_string(f_number aperture) {
  if (aperture.value == 0xffff)
    return "---";
  else if (aperture)
    return string_format("%.1ff", static_cast<double>(aperture) / 100.0);
  else
    return "Auto";
}

double ss_to_microsec(uint32_t raw_speed) {
  /* convert shutter speed raw value (obtained from camera) to microseconds */
  if (raw_speed & shutter_flag_subsecond)
    return 1000.0 * (1000000.0 / static_cast<double>(raw_speed & shutter_value_mask));
  else
    return 1000.0 * static_cast<double>(raw_speed);
}

void print(current_properties& settings) {
    printf("camera settings:\n");

    for (size_t i = 0; i < settings.camera_order.size(); ++i) {
        auto const &key   = settings.camera_order[i];
        auto const &value = settings.values[key];

        if (!is_known_property(key)) {
            printf("\t%s (%s): %s\n", to_string(property_unknown).c_str(),
                   hex_format(&key, 2).c_str(), hex_format(&value, 4).c_str());
            continue;
        }

        printf("\t%s: ", to_string(key).c_str());
        if (key == property_iso || key == property_movie_iso) {
            iso_level const iso { value };
            printf("%s\n", to_string(iso).c_str());
        } else if (key == property_shutter_speed) {
            shutter_speed const spd { value };
            printf(" %s\n", to_string(spd).c_str());
        } else if (key == property_focus_point) {
            auto_focus_point point = value;
            printf("%s\n", to_string(point).c_str());
        } else if (key == property_aperture) {
            f_number aperture = value;
            printf("%s\n", to_string(aperture).c_str());
        } else if (key == property_exposure_compensation) {
            printf("%.1f\n", static_cast<double>(static_cast<int16_t>(value)) / 1000.0);
        } else if (key == property_movie_remaining_time || key == property_image_space_sd) {
            printf("%d\n", value);
        } else if (is_known_property_value(key, value)) {
            printf("%s\n", to_string(key, value).c_str());
        } else {
            printf("%s (%d 0x%x)\n", unknown_value_str, value, value);
        }
    }
}

}  // namespace fcwt
