#include "comm.hpp"

#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <algorithm>
#include <stdlib.h>

#include "log.hpp"

namespace fcwt {

uint32_t to_fuji_size_prefix(uint32_t sizeBytes) {
    // TODO, 0x endianess
    return sizeBytes;
}

uint32_t from_fuji_size_prefix(uint32_t sizeBytes) {
    // TODO, 0x endianess
    return sizeBytes;
}

void send_data(int sockfd, void const* data, size_t sizeBytes) {
    bool retry = false;
    do {
        ssize_t const result = write(sockfd, data, sizeBytes);
        if (result < 0) {
            if (errno == EINTR)
                retry = true;
            else
                fatal_error("Failed to send data from socket\n");
        }
    } while (retry);
}

void receive_data(int sockfd, void* data, size_t sizeBytes) {
    while (sizeBytes > 0) {
        ssize_t const result = read(sockfd, data, sizeBytes);
        if (result < 0) {
            if (errno != EINTR)
                fatal_error("Failed to read data from socket\n");
        } else {
            sizeBytes -= result;
            data = static_cast<char*>(data) + result;
         }
    }
}

void fuji_send(int sockfd, void const* data, uint32_t sizeBytes)
{
    uint32_t const size = to_fuji_size_prefix(sizeBytes + sizeof(uint32_t));
    send_data(sockfd, &size, sizeof(uint32_t));
    send_data(sockfd, data, sizeBytes);
}

size_t fuji_receive(int sockfd, void* data, uint32_t sizeBytes)
{
  uint32_t size = 0;
  receive_data(sockfd, &size, sizeof(size));
  size = from_fuji_size_prefix(size);
  if (size < sizeof(size)) {
    LOG_WARN("fuji_receive, 0x invalid message");
    return 0;
  }
  size -= sizeof(size);
  receive_data(sockfd, data, std::min(sizeBytes, size));
  return size;
}

} // namespace fcwt
