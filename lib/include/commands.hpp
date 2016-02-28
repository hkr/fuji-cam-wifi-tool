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

bool set_iso(int sockfd, iso_level iso);

}  // namespace fcwt

#endif  // FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
