#include "log.hpp"

#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <memory>

namespace fcwt {

void print_hex(void const* data, size_t const sizeBytes, append_newline anl) {
  size_t i = 0;
  while (i < sizeBytes) {
    printf("%02X", static_cast<unsigned char const*>(data)[i]);
    ++i;
    if (i == sizeBytes) break;

    if (i % 4 == 0)
      printf(" ");
    else
      printf(":");
  }

  if (anl == newline) printf("\n");
}

void print_ascii(void const* data, size_t const sizeBytes, append_newline anl) {
  for (size_t i = 0; i < sizeBytes; ++i) {
    char c = static_cast<char const*>(data)[i];
    if (c < ' ' || c > '~') c = '.';
    printf("%c", c);
  }

  if (anl == newline) printf("\n");
}

void print_uint32(void const* data, size_t sizeBytes, append_newline anl) {
  auto bytes = static_cast<uint8_t const*>(data);
  int const numInts = sizeBytes / 4;
  for (int i = 0; i < numInts; ++i) {
    uint32_t u;
    memcpy(&u, &bytes[i * 4], 4);
    if (i > 0)
      printf("%u\n", u);
    else
      printf("%u\n", u);
  }

  // remaining bytes
  auto const remainingBytes = sizeBytes % 4;
  if (remainingBytes > 0) printf(" ");
  print_hex(&bytes[numInts * 4], remainingBytes, anl);
}

void fatal_error(char const* msg) {
  perror(msg);
  abort();
}

std::string string_format(char const* format, ...) {
  va_list args;
  va_start(args, format);
  size_t const size = std::snprintf(nullptr, 0, format, args) + 1;
  std::unique_ptr<char[]> buf(new char[size]);
  std::snprintf(buf.get(), size, format, args);
  va_end(args);
  return std::string(buf.get(), buf.get() + size - 1);
}

}  // namespace fcwt
