#include "capabilities.hpp"
#include "settings.hpp"
#include "log.hpp"

#include <stdio.h>

namespace fcwt {

typedef std::pair<property_codes, const char*> prop_str_pair;
const prop_str_pair property_strings[] = {
   {property_white_balance, "White Balance"},
   {property_aperture, "Aperture"},
   {property_focus_mode, "Focus Mode"},
   {property_shooting_mode, "Shooting Mode"},
   {property_flash, "Flash"},
   {property_exposure_compensation, "Exposure Compensation"},
   {property_self_timer, "Self Timer"},
   {property_film_simulation, "Film Simulation"},
   {property_image_format, "Image Format"},
   {property_recmode_enable, "Recording Mode"},
   {property_f_ss_control, "Aperture/ShutterSpeed ctrl"},
   {property_iso, "ISO"},
   {property_movie_iso, "Movie ISO"},
   {property_focus_point, "Focus Point"},
   {property_focus_lock, "Focus Lock"},
   {property_device_error, "Device Error"},
   {property_image_space_sd, "Image Space on SD"},
   {property_movie_remaining_time, "Movie Time Remaining"},
   {property_shutter_speed, "Shutter Speed"},
   {property_image_aspect, "Image Aspect"},
   {property_battery_level, "Battery Level"},
   {property_unknown, "== Unknown Property =="}
};

bool is_known_property(uint16_t value)
{
    auto it = std::find_if(std::begin(property_strings), std::end(property_strings), [value](prop_str_pair p){
        return p.first == value;
    });
    return it != std::end(property_strings);
}

std::string to_string(property_codes property)
{
    auto it = std::find_if(std::begin(property_strings), std::end(property_strings), [property](prop_str_pair p){
        return p.first == property;
    });

    if (it == std::end(property_strings))
        return std::to_string(property);

    return it->second;
}

#define PRINT_CAPABILITY(value, value_string, default_value, current_value) \
            std::string flag = ""; \
            if (value == current_value && value == default_value) \
                flag = "(default/current)"; \
            else if (value == default_value) \
                flag = "(default)"; \
            else if (value == current_value) \
                flag = "(current)"; \
            printf("\t\t%s %s\n", value_string, flag.c_str())

void print(std::vector<capability> const& caps) {
    printf("camera capabilities:\n");

    for (capability cap : caps) {
        if (!is_known_property(cap.property_code)) {
            printf("\t%s: %s\n", to_string(property_unknown).c_str(), hex_format(&cap.property_code, 2).c_str());
            continue;
        }

        printf("\t%s%s:\n", to_string(cap.property_code).c_str(), cap.get_set ? "" : " (immutable)");
/*
        if (cap.form_flag == 1) {
            printf("\t\t    min: %d\n", cap.min_value);
            printf("\t\t    max: %d\n", cap.max_value);
            printf("\t\t    step: %d\n", cap.step_size);
        } else if (cap.form_flag == 2) {
            for (uint16_t i = 0; i < cap.count; ++i) {
                printf("\t\t%d\n", cap.values[i]);
            }
        }
*/
        if (cap.property_code == property_exposure_compensation) {
            for (uint16_t i = 0; i < cap.count; ++i) {
                int16_t raw_mode = static_cast<int16_t>(cap.values[i]);
                std::string mode = string_format("%.1f", static_cast<double>(raw_mode) / 1000.0);
                PRINT_CAPABILITY(raw_mode, mode.c_str(),
                                 static_cast<int16_t>(cap.default_value),
                                 static_cast<int16_t>(cap.current_value));
            }

        } else if (cap.property_code == property_aperture) {
            for (uint16_t i = 0; i < cap.count; ++i) {
                uint16_t raw_mode = cap.values[i];
                f_number aperture = raw_mode;
                PRINT_CAPABILITY(raw_mode, to_string(aperture).c_str(),
                                 cap.default_value, cap.current_value);
            }

        } else if (cap.property_code == property_self_timer ||
                   cap.property_code == property_flash ||
                   cap.property_code == property_film_simulation ||
                   cap.property_code == property_recmode_enable ||
                   cap.property_code == property_white_balance) {
            for (uint16_t i = 0; i < cap.count; ++i) {
                std::string value_str = to_string(cap.property_code, cap.values[i]);
                value_str.append(string_format(" (%d)", cap.values[i]));
                PRINT_CAPABILITY(cap.values[i], value_str.c_str(),
                                 cap.default_value, cap.current_value);
            }

        } else if (cap.property_code == property_iso) {
            for (uint32_t i = 0; i < cap.count; ++i) {
                iso_level const lvl { cap.values[i] };
                printf("\t\t%s\n", to_string(lvl).c_str());
            }

        } else if (cap.property_code == property_shutter_speed) {
            shutter_speed const spd { cap.current_value };
            printf("\t\tvalue: %s\n", to_string(spd).c_str());

        } else if (cap.property_code == property_focus_point) {

        }
    }
}

} // namespace fcwt
