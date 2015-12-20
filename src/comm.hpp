#ifndef FUJI_CAM_WIFI_TOOL_COMM_HPP
#define FUJI_CAM_WIFI_TOOL_COMM_HPP

#include <stdint.h>
#include <stddef.h>

namespace fcwt {

void init_connection();

uint32_t to_fuji_size_prefix(uint32_t sizeBytes);
uint32_t from_fuji_size_prefix(uint32_t sizeBytes);

void send_data(int sockfd, void const* data, size_t sizeBytes);
void receive_data(int sockfd, void* data, size_t sizeBytes);
void fuji_send(int sockfd, void const* data, uint32_t sizeBytes);

// returns the total payload bytes, if this is more than sizeBytes the caller needs to use receive_data to get the additional data
size_t fuji_receive(int sockfd, void* data, uint32_t sizeBytes);

template <size_t N>
static void fuji_send(int sockfd, uint8_t (&data)[N]) {
    fuji_send(sockfd, data, N);
}

} // namespace fcwt

#endif
