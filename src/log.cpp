#include "log.hpp"

#include <stdlib.h>

namespace fcwt {

void print_hex(void const* data, size_t const sizeBytes)
{
    for (size_t i = 0; i < sizeBytes; ++i)
        printf("%02X", static_cast<unsigned char const*>(data)[i]);
    printf("\n");
}

void fatal_error(char const* msg) {
    perror(msg);    
    abort();
}

} // namespace fcwt
