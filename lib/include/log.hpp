#ifndef FUJI_CAM_WIFI_TOOL_LOG_HPP
#define FUJI_CAM_WIFI_TOOL_LOG_HPP

#include <stdio.h>
#include <string>

#include "platform.hpp"

namespace fcwt {

const uint8_t LOG_ERROR = 1;
const uint8_t LOG_WARN = 2;
const uint8_t LOG_INFO = 3;
const uint8_t LOG_DEBUG = 4;
const uint8_t LOG_DEBUG2 = 5;

const uint8_t LOG_STDOUT = 1;
const uint8_t LOG_FILE = 2;

struct log_settings {
    uint8_t level = LOG_DEBUG;
    uint8_t output = LOG_STDOUT;
};

void log(uint8_t level, std::string msg);

enum append_newline { skip_newline, newline };

std::string hex_format(void const* data, size_t const sizeBytes);

void print_ascii(void const* data, size_t const sizeBytes,
                 append_newline anl = newline);
void print_uint32(void const* data, size_t const sizeBytes,
                  append_newline anl = newline);
void fatal_error(char const* msg);

FCWT_FORMAT_PRINTF_FUNCTION std::string string_format(
	FCWT_FORMAT_STRING(char const* format), ...);

}  // namespace fcwt

#endif
