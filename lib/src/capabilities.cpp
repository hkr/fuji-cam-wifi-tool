#include "capabilities.hpp"
#include "settings.hpp"
#include "log.hpp"

#include <stdio.h>

namespace fcwt {

std::string to_string(aperture_f_number aperture)
{
    if (aperture.value)
        return string_format("%.1ff", static_cast<float>(aperture.value) / 100.f);
    else
        return "auto";
}

#define PRINT_CAPABILITY(raw_mode, mode, min_value, value) \
            std::string flag = ""; \
            if (raw_mode == value && value == min_value) \
                flag = "(minimum/current)"; \
            else if (raw_mode == min_value) \
                flag = "(minimum)"; \
            else if (raw_mode == value) \
                flag = "(current)"; \
            printf("\t\t%s %s\n", mode, flag.c_str())

void print(camera_capabilities const& caps)
{
    printf("camera capabilities:\n");

    printf("\tiso levels (%d):\n", caps.iso.count);
    for (uint32_t i = 0; i < caps.iso.count; ++i)
    {
        iso_level const lvl { caps.iso.modes[i] };
        printf("\t\t%s\n", to_string(lvl).c_str());
    }

    printf("\tshutter speed:\n");
    shutter_speed const spd { caps.shutter.value };
    printf("\t\tvalue: %s\n", to_string(spd));

    printf("\taperture modes:\n");
    for (uint16_t i = 0; i < caps.aperture.count; ++i) {
        uint16_t raw_mode = caps.aperture.modes[i];
        double mode = static_cast<double>(raw_mode) / 100.0;

        if (raw_mode == caps.aperture.value &&
              raw_mode == caps.aperture.min_value)
            printf("\t\t%.1ff (minimum/current)\n", mode);
        else if (raw_mode == caps.aperture.min_value)
            printf("\t\t%.1ff (minimum)\n", mode);
        else if (raw_mode == caps.aperture.value)
            printf("\t\t%.1ff (current)\n", mode);
        else
            printf("\t\t%.1ff\n", mode);
    }

    printf("\tflash modes (%d):\n", caps.flash.count);
    for (uint16_t i = 0; i < caps.flash.count; ++i) {
        flash_mode mode = static_cast<flash_mode>(caps.flash.modes[i]);
        PRINT_CAPABILITY(caps.flash.modes[i], to_string(mode),
                         caps.flash.min_value, caps.flash.value);
    }

    printf("\twhite balance modes (%d):\n", caps.white_balance.count);
    for (uint16_t i = 0; i < caps.white_balance.count; ++i) {
        white_balance_mode mode = static_cast<white_balance_mode>(caps.white_balance.modes[i]);
        PRINT_CAPABILITY(caps.white_balance.modes[i], to_string(mode),
                         caps.white_balance.min_value, caps.white_balance.value);
    }

    printf("\tfilm simulation modes (%d):\n", caps.film_simulation.count);
    for (uint16_t i = 0; i < caps.film_simulation.count; ++i) {
        film_simulation_mode mode = static_cast<film_simulation_mode>(caps.film_simulation.modes[i]);
        PRINT_CAPABILITY(caps.film_simulation.modes[i], to_string(mode),
                         caps.film_simulation.min_value, caps.film_simulation.value);
    }

    printf("\trecording modes (%d):\n", caps.recording.count);
    for (uint16_t i = 0; i < caps.recording.count; ++i) {
        recording_mode mode = static_cast<recording_mode>(caps.recording.modes[i]);
        PRINT_CAPABILITY(caps.recording.modes[i], to_string(mode),
                         caps.recording.min_value, caps.recording.value);
    }

    printf("\tself timer modes (%d):\n", caps.self_timer.count);
    for (uint16_t i = 0; i < caps.self_timer.count; ++i) {
        timer_mode mode = static_cast<timer_mode>(caps.self_timer.modes[i]);
        PRINT_CAPABILITY(caps.self_timer.modes[i], to_string(mode),
                         caps.self_timer.min_value, caps.self_timer.value);
    }

    printf("\texposure compensation modes (%d):\n", caps.exposure_compensation.count);
    for (uint16_t i = 0; i < caps.exposure_compensation.count; ++i) {
        int16_t raw_mode = static_cast<int16_t>(caps.exposure_compensation.modes[i]);
        double mode = static_cast<double>(raw_mode) / 1000.0;

        if (caps.exposure_compensation.modes[i] == caps.exposure_compensation.value &&
                 caps.exposure_compensation.modes[i] == caps.exposure_compensation.min_value)
            printf("\t\t%.1f (%d) (minimum/current)\n", mode, raw_mode);
        else if (caps.exposure_compensation.modes[i] == caps.exposure_compensation.min_value)
            printf("\t\t%.1f (%d) (minimum)\n", mode, raw_mode);
        else if (caps.exposure_compensation.modes[i] == caps.exposure_compensation.value)
            printf("\t\t%.1f (%d) (current)\n", mode, raw_mode);
        else
            printf("\t\t%.1f (%d)\n", mode, raw_mode);
    }

}

} // namespace fcwt
