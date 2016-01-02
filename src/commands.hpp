#ifndef FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
#define FUJI_CAM_WIFI_TOOL_COMMANDS_HPP

#include <stdint.h>
#include "comm.hpp"
#include "log.hpp"

namespace fcwt {

struct message
{
    uint16_t index = 1;   // index if this is a multi-part message, not really surea bout the second byte
    uint8_t type[2];  // not sure about this, might be two separate fields
    uint32_t id;
};

uint32_t generate_message_id();
bool init_control_connection(int sockfd, char const* deviceName);

bool is_success_response(uint32_t const id, void const* buffer, uint32_t const size);

bool fuji_message(int const sockfd, uint32_t const id, void* message, size_t size);

template <size_t N>
bool fuji_message(int const sockfd, uint32_t const id, uint8_t(&msg)[N])
{
    return fuji_message(sockfd, id, msg, N);
}

} // namespace fcwt

#endif
