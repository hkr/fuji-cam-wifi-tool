#ifndef FUJI_CAM_WIFI_TOOL_LOG_HPP
#define FUJI_CAM_WIFI_TOOL_LOG_HPP

#include <stdio.h>
#include <string>

#define LOG_INFO(str) printf("[INFO] " str "\n")
#define LOG_INFO_FORMAT(fmt, ...) printf("[INFO] " fmt "\n", __VA_ARGS__)

#define LOG_WARN(str) printf("[WARNING] " str "\n")
#define LOG_WARN_FORMAT(fmt, ...) printf("[WARNING] " fmt "\n", __VA_ARGS__)

namespace fcwt {

enum append_newline { skip_newline, newline };

void print_hex(void const* data, size_t const sizeBytes, append_newline anl = newline);
void print_ascii(void const* data, size_t const sizeBytes, append_newline anl = newline);
void print_uint32(void const* data, size_t const sizeBytes, append_newline anl = newline);
void fatal_error(char const* msg);

__attribute__((format(printf,1,2))) std::string string_format(char const* format, ...);

} // namespace fcwt

#endif
