#include "comm.hpp"

#include <stdint.h>
#include <errno.h>
#include <algorithm>
#include <vector>
#include <cstring>
#include <assert.h>

#if FCWT_USE_BSD_SOCKETS
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#elif FCWT_USE_WINSOCK
#define NOMINMAX
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif

#include "log.hpp"

namespace fcwt {

const char* const server_ipv4_addr = "192.168.0.1";

namespace {
#if FCWT_USE_WINSOCK
	void print_socket_api_error()
	{
		int  err = WSAGetLastError();
		char* s = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&s, 0, NULL);
		printf("%s\n", s);
		LocalFree(s);
	}

	WSADATA* api_init()
	{
		struct wsa_data_holder
		{
			wsa_data_holder()
				: valid(false)
				, data()
			{
				int ver = MAKEWORD(2, 2);
				valid = WSAStartup(ver, &data) == 0;
				if (!valid)
					print_socket_api_error();
			}
			~wsa_data_holder()
			{
				if (!valid)
					return;

				if (WSACleanup() != 0)
					print_socket_api_error();
			};
			WSADATA data;
			bool valid;
		};
		static wsa_data_holder wsa;
		return wsa.valid ? &wsa.data : nullptr;
	}
#else
	void print_socket_api_error() {} // TODO
	void api_init() {}
#endif
} // namespace

static void close_socket(native_socket sockfd)
{
	if (sockfd)
	{
#if FCWT_USE_WINSOCK
		closesocket(sockfd);
#elif FCWT_USE_WINSOCK
		close(sockfd);
#endif
	}
}

sock::sock(native_socket fd)
	: sockfd(fd)
{
}

sock::~sock()
{
	close_socket(sockfd);
}

sock::sock(sock&& other)
	: sockfd(other.sockfd)
{
	other.sockfd = 0;
}

sock::operator native_socket() const
{
	return sockfd;
}

sock& sock::operator=(sock&& other) {
  sock tmp(std::move(other));
  swap(tmp);
  return *this;
}

void sock::swap(sock& other) {
	using std::swap;
	swap(sockfd, other.sockfd);
}

static void set_nonblocking_io(native_socket sockfd, bool nonblocking)
{
#if FCWT_USE_BSD_SOCKETS
	fcntl(sockfd, F_SETFL, nonblocking ? O_NONBLOCK : 0);  // for timeout
#elif FCWT_USE_WINSOCK
	u_long arg = nonblocking ? 1 : 0;
	ioctlsocket(sockfd, FIONBIO, &arg);
#endif
}

sock connect_to_camera(int port) {
  // TODO: proper error handling

  api_init();

  const native_socket sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) fatal_error("Failed to create socket\n");

  set_nonblocking_io(sockfd, true);  // for timeout

  sockaddr_in sa = {};
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
#if FCWT_USE_BSD_SOCKETS
  inet_pton(AF_INET, server_ipv4_addr, &sa.sin_addr);
#elif FCWT_USE_WINSOCK
  InetPton(AF_INET, server_ipv4_addr, &sa.sin_addr);
#else
#error need inet_pton
#endif
  connect(sockfd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa));

  // timeout handling
  fd_set fdset;
  FD_ZERO(&fdset);
  FD_SET(sockfd, &fdset);
  struct timeval tv = {};
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  if (select(static_cast<int>(sockfd + 1), NULL, &fdset, NULL, &tv) == 1) {
    int so_error = 0;
    socklen_t len = sizeof so_error;
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len);

    if (so_error == 0) {
      log(LOG_INFO, string_format("Connection esatablished %s:%d (%lld)",
                                  server_ipv4_addr, port, (long long) sockfd));
      set_nonblocking_io(sockfd, false);
      return sockfd;
    }
  }

  log(LOG_ERROR, "Failed to connect");
  close_socket(sockfd);
  
  return 0;
}

static uint32_t to_fuji_size_prefix(uint32_t sizeBytes) {
  // TODO, 0x endianess
  return sizeBytes;
}

static uint32_t from_fuji_size_prefix(uint32_t sizeBytes) {
  // TODO, 0x endianess
  return sizeBytes;
}

void send_data(native_socket sockfd, void const* data, size_t sizeBytes) {
  bool retry = false;
  do {
#if FCWT_USE_BSD_SOCKETS
	ssize_t const result = write(sockfd, data, sizeBytes);
#elif FCWT_USE_WINSOCK
	int const result = send(sockfd, static_cast<char const*>(data), static_cast<int>(sizeBytes), 0);
#endif
    if (result < 0) {
      if (errno == EINTR)
        retry = true;
      else
        fatal_error("Failed to send data from socket\n");
    }
  } while (retry);
}

void receive_data(native_socket sockfd, void* data, size_t sizeBytes) {
  while (sizeBytes > 0) {
#if FCWT_USE_BSD_SOCKETS
    ssize_t const result = read(sockfd, data, sizeBytes);
#elif FCWT_USE_WINSOCK
	int const result = recv(sockfd, static_cast<char*>(data), static_cast<int>(sizeBytes), 0);
#endif
    if (result < 0) {
      if (errno != EINTR) fatal_error("Failed to read data from socket\n");
    } else {
      sizeBytes -= result;
      data = static_cast<char*>(data) + result;
    }
  }
}

void fuji_send(native_socket sockfd, void const* data, size_t sizeBytes) {
  std::vector<uint8_t> msg(sizeof(uint32_t) + sizeBytes);
  *((uint32_t *)msg.data()) = to_fuji_size_prefix(msg.size());
  std::memcpy(msg.data() + 4, data, sizeBytes);
  send_data(sockfd, msg.data(), msg.size());
}

size_t fuji_receive(native_socket sockfd, void* data, size_t sizeBytes) {
  uint32_t size = 0;
  receive_data(sockfd, &size, sizeof(size));
  size = from_fuji_size_prefix(size);
  if (size < sizeof(size)) {
    log(LOG_WARN, "fuji_receive, 0x invalid message");
    return 0;
  }
  size -= sizeof(size);
  receive_data(sockfd, data, std::min(sizeBytes, static_cast<size_t>(size)));
  // if size == 4 and data = 0xffffffff then indicates an error or busy)
  return size;
}

}  // namespace fcwt
