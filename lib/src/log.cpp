#include "log.hpp"

#include <stdlib.h>
#include <algorithm>

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

void print_uint32(void const* data, size_t sizeBytes, append_newline anl)
{
	// unaligned bytes
	size_t unalignedByteCount = std::min(reinterpret_cast<uintptr_t>(data) % sizeof(uintptr_t), sizeBytes);
	print_hex(data, unalignedByteCount, skip_newline);

	if (unalignedByteCount >= sizeBytes)
		return;

	if (unalignedByteCount > 0)
		printf(" ");

	data = static_cast<uint8_t const*>(data) + unalignedByteCount;
	sizeBytes -= unalignedByteCount;
	auto const ints = static_cast<uint32_t const*>(data);

	// actual ints
	int const numInts = sizeBytes / 4;
    for (int i = 0; i < numInts; ++i)
    {
    	if (i > 0)
    		printf("%u", ints[i]);
    	else
        	printf(" %u", ints[i]);
    }

    // remaining bytes
    auto const remainingBytes = sizeBytes % 4;
    if (remainingBytes > 0)
    	printf(" ");
    print_hex(&ints[numInts], sizeBytes % 4, anl);
}

void fatal_error(char const* msg)
{
    perror(msg);    
    abort();
}

} // namespace fcwt
