#include "settings.hpp"

#include "capabilities.hpp"
#include "log.hpp"

#include <stdio.h>

namespace fcwt {

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

char const* to_string(shutter_speed speed) {
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

void print(std::map<property_codes, uint32_t> const& settings) {
    printf("camera settings:\n");

    for (auto const &s : settings) {
        auto const &key   = s.first;
        auto const &value = s.second;

        if (! property_strings.count(key)) {
            printf("\t%s (%s): %s\n", property_strings[property_unknown],
                   hex_format(&key, 2).c_str(), hex_format(&value, 4).c_str());
            continue;
        }

        printf("\t%s: ", property_strings[key]);
        if (key == property_iso || key == property_movie_iso) {
            iso_level const iso { value };
            printf("%s\n", to_string(iso).c_str());
        } else if (key == property_shutter_speed) {
            shutter_speed const spd { value };
            printf(" %s\n", to_string(spd));
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
        } else if (property_value_strings[key].count(value)) {
            printf("%s\n", property_value_strings[key][value]);
        } else {
            printf("%s\n", property_value_strings[property_unknown][0]);
        }
    }
}

}  // namespace fcwt
