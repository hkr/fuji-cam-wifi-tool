#include "log.hpp"

#include <stdlib.h>

namespace fcwt {

void print_hex(void const* data, size_t const sizeBytes, append_newline anl)
{
    size_t i = 0;
    for (;;)
    {
        printf("%02X", static_cast<unsigned char const*>(data)[i]);
        ++i;
        if (i == sizeBytes)
            break;

        if (i % 4 == 0)
            printf(" ");
        else
            printf(":");
    }

    if (anl == newline)
        printf("\n");
}

void fatal_error(char const* msg)
{
    perror(msg);    
    abort();
}

} // namespace fcwt
