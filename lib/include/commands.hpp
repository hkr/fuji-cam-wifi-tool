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
                             std::vector<capability>* caps);
void terminate_control_connection(native_socket sockfd);

bool shutter(native_socket const sockfd, native_socket const sockfd2, const char* thumbnail = 0);

bool current_settings(native_socket sockfd, current_properties& settings);

enum fnumber_update_direction {
    fnumber_increment,
    fnumber_decrement
};

enum ss_update_direction {
    ss_increment,
    ss_decrement
};

enum exp_update_direction {
    exp_increment,
    exp_decrement
};

bool update_setting(native_socket sockfd, property_codes code, uint32_t value);
bool update_setting(native_socket sockfd, auto_focus_point point);
bool update_setting(native_socket sockfd, fnumber_update_direction dir);
bool update_setting(native_socket sockfd, ss_update_direction dir);
bool update_setting(native_socket sockfd, exp_update_direction dir);
bool unlock_focus(native_socket sockfd);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
