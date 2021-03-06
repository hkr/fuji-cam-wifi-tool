#ifndef FUJI_CAM_WIFI_TOOL_COMM_HPP
#define FUJI_CAM_WIFI_TOOL_COMM_HPP

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <memory>

namespace fcwt {

const int control_server_port = 55740;
const int async_response_server_port = 55741;
const int jpg_stream_server_port = 55742;

#ifdef _WIN32
#define FCWT_USE_WINSOCK 1
#elif defined(__unix__) || defined(__MACH__) || defined(__linux__)
#define FCWT_USE_BSD_SOCKETS 1
#endif

#if FCWT_USE_BSD_SOCKETS
	typedef int native_socket;
#elif FCWT_USE_WINSOCK
	typedef uintptr_t native_socket;
#else
#	error unsupported platform
#endif

class sock {
  native_socket sockfd;

 public:
  sock(native_socket fd = 0);
  ~sock();
  sock(sock const&) = delete;
  sock& operator=(sock const&) = delete;
  sock(sock&& other);
  sock& operator=(sock&& other);
  void swap(sock& other);
  operator native_socket() const;
};

sock connect_to_camera(int port);

void send_data(native_socket sockfd, void const* data, size_t sizeBytes);
void receive_data(native_socket sockfd, void* data, size_t sizeBytes);
void fuji_send(native_socket sockfd, void const* data, size_t sizeBytes);

// returns the total payload bytes, if this is more than sizeBytes the caller
// needs to use receive_data to get the additional data
size_t fuji_receive(native_socket sockfd, void* data, size_t sizeBytes);

template <size_t N>
void fuji_send(native_socket sockfd, uint8_t const(&data)[N]) {
  fuji_send(sockfd, data, N);
}

template <size_t N>
size_t fuji_receive(native_socket sockfd, uint8_t(&data)[N]) {
  return fuji_receive(sockfd, data, N);
}

}  // namespace fcwt

#endif
