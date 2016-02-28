#include "capabilities.hpp"

#include <stdio.h>
#include "log.hpp"

namespace fcwt {

std::string to_string(iso_level iso)
{
    auto const lvl = iso.value;
    char const* flag = "";
    if (lvl & iso_flag_auto) 
        flag = " (auto)";
    else if (lvl & iso_flag_emulated)
        flag = " (emulated)";

    return string_format("%12d%s", lvl & iso_value_mask, flag);
}

std::string to_string(aperture_f_number aperture)
{
    if (aperture.value)
        return string_format("%.1f", static_cast<float>(aperture.value) / 100.f);
    else
        return "auto";
}

void print(camera_capabilities const& caps)
{
    printf("camera capabilities:\n");

    printf("\tiso levels (%d):\n", caps.iso.numLevels);
    for (uint32_t i = 0; i < caps.iso.numLevels; ++i)
    {
        iso_level const lvl { caps.iso.levels[i] };
        printf("\t\t%s\n", to_string(lvl).c_str());
    }

    printf("\tshutter speed:\n");
    printf("\t\tmode: %s\n", caps.shutter_speed.mode == shutter_speed_auto ? "auto" : "manual");
    if (caps.shutter_speed.mode == shutter_speed_manual)
        printf("\t\ttime: 1/%ds\n", caps.shutter_speed.value);
    else
        printf("\t\texposure compensation: %d\n", static_cast<int>(caps.shutter_speed.exposure));
    printf("\taperture:\n");
    printf("\t\tvalue: %s\n", to_string(caps.aperture).c_str());
}

} // namespace fcwt
