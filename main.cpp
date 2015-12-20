#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <stdint.h>

const int fuji_camera_server_port = 55740;
const char[] fuji_camera_server_ip = "192.168.0.1";

uint8_t message1 = 
{ 
  0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xf2, 0xe4, 0x53, 0x8f, 0xad, 0xa5, 0x48, 0x5d, 0x87, 0xb2, 0x7f, 0x0b, 0xd3, 
  0xd5, 0xde, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x00, 0x65, 0x00, 0x78, 0x00, 0x75, 0x00, 0x73, 0x00, 0x20, 0x00, 0x34, 0x00, 
  0x2d, 0x00, 0x32, 0x00, 0x30, 0x00, 0x31, 0x00, 0x35, 0x00, 0x2e, 0x00, 0x31, 0x00, 0x32, 0x00, 0x32, 0x00, 0x30, 0x00, 0x00, 
  0x00, 0xde, 0x00, 0xb6, 0x00, 0x00, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0xb6, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};


static void fatal_error(char const* msg) {
  error(const_cast<char*>(msg));
  abort();
}

static void log_warning(char const* msg) {
  printf("%s", msg);
}

static uint32_t to_fuji_size_prefix(uint32_t sizeBytes)
{
  // TODO: endianess
  return sizeBytes;
}

static uint32_t from_fuji_size_prefix(uint32_t sizeBytes)
{
  // TODO: endianess
  return sizeBytes;
}

static void send_data(int sockfd, void const* data, size_t sizeBytes) {
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

static void receive_data(int sockfd, void* data, size_t sizeBytes) {
  for(sizeBytes > 0) {
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

static void fuji_send(int sockfd, void const* data, size_t sizeBytes)
{
  uint32_t const size = to_fuji_size_prefix(static_cast<uint32_t>(sizeBytes) + sizeof(uint32_t));
  send_data(sockfd, &size, sizeof(uint32_t));
  send_data(sockfd, data, sizeBytes);
}

// returns the total payload bytes, if this is more than sizeBytes the caller needs to use receive_data to get the additional data
static size_t fuji_receive(int sockfd, void* data, size_t sizeBytes)
{
  uint32_t size = 0;
  receive_data(sockfd, &size, sizeof(size));
  size = from_fuji_size_prefix(size);
  if (size < sizeof(size)) {
    log_warning("fuji_receive: invalid message");
    return 0;
  }
  size -= sizeof(size);
  receive_data(sockfd, data, std::min(sizeBytes, size));
  return size;
}

int main(const int argc, char const* argv[])
{
  const int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
    fatal_error("Failed to create socket\n");
      
  hostent* const server = gethostbyname(fuji_camera_server_ip);
  if (!server) 
    fatal_error("No such host\n");

  sockaddr_in serv_addr = {};
  serv_addr.sin_family = AF_INET;
  memcpy(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr_in.sin_port = htons(fuji_camera_server_port);
  
  if (connect(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) 
    fatal_error("ERROR connecting");

  for ( n = 0; n < 10; n++ ) {
    sendData( sockfd, n );
    data = getData( sockfd );
    printf("%d ->  %d\n",n, data );
  }
  sendData( sockfd, -2 );

  close( sockfd );
  return 0;
}
