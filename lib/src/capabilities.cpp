#include "capabilities.hpp"
#include "settings.hpp"
#include "log.hpp"

#include <stdio.h>

namespace fcwt {

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
        if (! property_strings.count(cap.property_code)) {
            printf("\t%s: %s\n", property_strings[property_unknown], hex_format(&cap.property_code, 2).c_str());
            continue;
        }

        printf("\t%s%s:\n", property_strings[cap.property_code], cap.get_set ? "" : " (immutable)");
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
                std::string mode = string_format("%.1ff", static_cast<double>(raw_mode) / 100.0);
                PRINT_CAPABILITY(raw_mode, mode.c_str(),
                                 cap.default_value, cap.current_value);
            }

        } else if (cap.property_code == property_self_timer ||
                   cap.property_code == property_flash ||
                   cap.property_code == property_film_simulation ||
                   cap.property_code == property_recmode_enable ||
                   cap.property_code == property_white_balance) {
            for (uint16_t i = 0; i < cap.count; ++i) {
                PRINT_CAPABILITY(cap.values[i], property_value_strings[cap.property_code][cap.values[i]],
                                 cap.default_value, cap.current_value);
            }

        } else if (cap.property_code == property_iso) {
            for (uint32_t i = 0; i < cap.count; ++i) {
                iso_level const lvl { cap.values[i] };
                printf("\t\t%s\n", to_string(lvl).c_str());
            }

        } else if (cap.property_code == property_shutter_speed) {
            shutter_speed const spd { cap.current_value };
            printf("\t\tvalue: %s\n", to_string(spd));

        } else if (cap.property_code == property_focus_point) {

        }
    }
}

} // namespace fcwt
