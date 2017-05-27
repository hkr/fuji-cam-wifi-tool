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

bool init_control_connection(native_socket sockfd, char const* deviceName,
                             camera_capabilities* caps);
void terminate_control_connection(native_socket sockfd);

bool shutter(native_socket const sockfd);

bool current_settings(native_socket sockfd, camera_settings& settings);

enum aperture_f_stop {
    aperture_open_third_stop,
    aperture_close_third_stop
};

enum shutter_speed_stop {
	shutter_speed_one_stop_faster,
	shutter_speed_one_stop_slower
};

bool update_setting(native_socket sockfd, iso_level iso);
bool update_setting(native_socket sockfd, image_settings image);
bool update_setting(native_socket sockfd, film_simulation_mode film);
bool update_setting(native_socket sockfd, auto_focus_point point);
bool update_setting(native_socket sockfd, white_balance_mode white_balance);
bool update_setting(native_socket sockfd, aperture_f_stop aperture);
bool update_setting(native_socket sockfd, shutter_speed_stop shutter_speed);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
