#ifndef FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
#define FUJI_CAM_WIFI_TOOL_COMMANDS_HPP

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <vector>

#include "comm.hpp"
#include "log.hpp"
#include "capabilities.hpp"
#include "settings.hpp"
#include "message.hpp"

namespace fcwt {

bool init_control_connection(int sockfd, char const* deviceName,
                             camera_capabilities* caps);
void terminate_control_connection(int sockfd);

bool shutter(int const sockfd);

bool current_settings(int sockfd, camera_settings& settings);

enum aperture_f_stop {
    aperture_open_third_stop,
    aperture_close_third_stop
};

bool update_setting(int sockfd, iso_level iso);
bool update_setting(int sockfd, image_settings image);
bool update_setting(int sockfd, film_simulation_mode film);
bool update_setting(int sockfd, auto_focus_point point);
bool update_setting(int sockfd, white_balance_mode white_balance);
bool update_setting(int sockfd, aperture_f_stop aperture);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
