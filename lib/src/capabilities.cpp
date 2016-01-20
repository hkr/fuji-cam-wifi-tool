#include "capabilities.hpp"

#include <stdio.h>

namespace fcwt {

#if 0
const uint32_t iso_flag_auto = 1 << 31;
const uint32_t iso_flag_emulated = 1 << 30;
const uint32_t iso_value_mask = 0x00ffffff;
const uint32_t iso_max_levels = 32;

struct iso_caps
{
    uint32_t levels[iso_max_levels] = {};
    uint32_t numLevels = 0;
};

enum shutter_speed_mode
{
    shutter_speed_auto,
    shutter_speed_manual
};

struct shutter_speed_caps
{
    shutter_speed_mode mode = shutter_speed_auto;
    uint32_t value = 0;
    int32_t exposure = 0;
};

struct aperture_caps
{
    uint32_t value = 0;
};

struct auto_focus_caps
{
    // TODO
};

struct camera_capabilities
{
    iso_caps iso;
    shutter_speed_caps shutterSpeed;
    auto_focus_caps autoFocus;
    aperture_caps apterture;
};
#endif

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