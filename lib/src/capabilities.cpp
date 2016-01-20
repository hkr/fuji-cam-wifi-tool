#include "capabilities.hpp"

#include <stdio.h>

namespace fcwt {

void print(camera_capabilities const& caps)
{
    printf("camera capabilities:\n");

    printf("\tiso levels (%d):\n", caps.iso.numLevels);
    for (uint32_t i = 0; i < caps.iso.numLevels; ++i)
    {
        auto const lvl = caps.iso.levels[i];
        char const* flag = "";
        if (lvl & iso_flag_auto) 
            flag = " (auto)";
        else if (lvl & iso_flag_emulated)
            flag = " (emulated)";

        printf("\t\t%12d%s\n", lvl & iso_value_mask, flag);
    }

    printf("\tshutter speed:\n");
    printf("\t\tmode: %s\n", caps.shutter_speed.mode == shutter_speed_auto ? "auto" : "manual");
    if (caps.shutter_speed.mode == shutter_speed_manual)
        printf("\t\ttime: 1/%ds\n", caps.shutter_speed.value);
    else
        printf("\t\texposure: %d\n", static_cast<int>(caps.shutter_speed.exposure));
    printf("\taperture:\n");
    if (caps.aperture.value)
        printf("\t\tvalue: %.1f\n", static_cast<float>(caps.aperture.value) / 100.f);
    else
        printf("\t\tauto\n");
}

} // namespace fcwt