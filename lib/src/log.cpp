#include "log.hpp"

#include <stdarg.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <memory>

namespace fcwt {

log_settings log_s;

void log(uint8_t level, char const* msg) {
  if (level <= log_s.level)
    switch(level) {
      case LOG_ERROR: {
        printf("[ERROR] %s\n", msg);
        break;
      }
      case LOG_WARN: {
        printf("[WARN] %s\n", msg);
        break;
      }
      case LOG_INFO: {
        printf("[INFO] %s\n", msg);
        break;
      }
      case LOG_DEBUG: {
        printf("[DEBUG] %s\n", msg);
        break;
      }
    }
}

std::string hex_format(void const* data, size_t const sizeBytes) {
  std::string output = "[";
  size_t i = 0;

  while (i < sizeBytes) {
    output.append(string_format("%02X", static_cast<unsigned char const*>(data)[i]));
    ++i;
    if (i == sizeBytes) break;

    if (i % 4 == 0)
      output.append(" ");
    else
      output.append(":");
  }

  output.append("]");

  return output;
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
  size_t const numInts = sizeBytes / 4;
  for (size_t i = 0; i < numInts; ++i) {
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
  printf(hex_format(&bytes[numInts * 4], remainingBytes).c_str());
}

void fatal_error(char const* msg) {
  perror(msg);
  abort();
}

std::string string_format(char const* format, ...) {
  va_list args;
  va_start(args, format);
  int const length = std::vsnprintf(nullptr, 0, format, args);
  va_end(args);
  std::string result;
  if (length > 0)
  {
    size_t size = length + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    va_start(args, format);
    if (std::vsnprintf(buf.get(), size, format, args) == length)
      result.assign(buf.get(), buf.get() + size - 1);
    va_end(args);
  }
  
  return result;
}

}  // namespace fcwt
