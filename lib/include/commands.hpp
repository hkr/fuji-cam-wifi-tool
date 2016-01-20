#ifndef FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
#define FUJI_CAM_WIFI_TOOL_COMMANDS_HPP

#include "message.hpp"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "comm.hpp"
#include "log.hpp"

namespace fcwt {

bool init_control_connection(int sockfd, char const* deviceName);
void terminate_control_connection(int sockfd);

bool shutter(int const sockfd);
bool set_iso(int sockfd, uint32_t iso);

} // namespace fcwt

#endif // FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
