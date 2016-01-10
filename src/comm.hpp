#ifndef FUJI_CAM_WIFI_TOOL_COMM_HPP
#define FUJI_CAM_WIFI_TOOL_COMM_HPP

#include <stdint.h>
#include <stddef.h>
#include <string.h>

namespace fcwt {

const int control_server_port = 55740;
const int jpg_stream_server_port = 55742;

class sock
{
  int sockfd;
public:
  sock(int fd = 0);
  ~sock();
  sock(sock const&) = delete;
  sock& operator=(sock const&) = delete;
  sock(sock&& other);
  sock& operator=(sock&& other);
  void swap(sock& other);
  operator int() const { return sockfd; }
};

sock connect_to_camera(int port);

uint32_t to_fuji_size_prefix(uint32_t sizeBytes);
uint32_t from_fuji_size_prefix(uint32_t sizeBytes);

void send_data(int sockfd, void const* data, size_t sizeBytes);
void receive_data(int sockfd, void* data, size_t sizeBytes);
void fuji_send(int sockfd, void const* data, uint32_t sizeBytes);

// returns the total payload bytes, if this is more than sizeBytes the caller needs to use receive_data to get the additional data
size_t fuji_receive(int sockfd, void* data, uint32_t sizeBytes);

template <size_t N>
void fuji_send(int sockfd, uint8_t const (&data)[N])
{
    fuji_send(sockfd, data, N);
}

template <size_t N>
size_t fuji_receive(int sockfd, uint8_t (&data)[N])
{
    return fuji_receive(sockfd, data, N);
}

uint32_t generate_message_id();

void* fill_message_id(uint32_t id, void* buffer, size_t size);

template <size_t N>
uint8_t (&fill_message_id(uint32_t id, uint8_t(&buffer)[N]))[N]
{
    static_assert(N >= 8, "buffer must be larger than 8 bytes");
    memcpy(&buffer[4], &id, sizeof(id));
    return buffer;
} 

} // namespace fcwt

#endif
