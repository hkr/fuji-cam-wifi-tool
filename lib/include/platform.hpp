#ifndef FUJI_CAM_WIFI_TOOL_PLATFORM_HPP
#define FUJI_CAM_WIFI_TOOL_PLATFORM_HPP

#include <assert.h>

#ifdef _MSC_VER
#define FCWT_FORMAT_STRING(a) _Printf_format_string_ a
#define FCWT_UNREACHABLE __assume(0)
#elif __GNUC__
#define FCWT_FORMAT_PRINTF_FUNCTION __attribute__((format(printf, 1, 2)))
#define FCWT_UNREACHABLE __builtin_unreachable()
#endif

#ifndef FCWT_FORMAT_PRINTF_FUNCTION
#define FCWT_FORMAT_PRINTF_FUNCTION
#endif

#ifndef FCWT_FORMAT_STRING
#define FCWT_FORMAT_STRING(s) s
#endif

#ifndef FCWT_UNREACHABLE
#define FCWT_UNREACHABLE assert(false)
#endif

#endif // FUJI_CAM_WIFI_TOOL_PLATFORM_HPP
