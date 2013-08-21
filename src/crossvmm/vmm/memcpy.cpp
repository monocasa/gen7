#include <cstdint>
#include <cstring>

void *memcpy( void *dest, const void *src, size_t n )
{
	uint8_t *cur_dst = (uint8_t*)dest;
	uint8_t *cur_src = (uint8_t*)src;

	for( size_t i = 0; i < n; i++ ) {
		*(cur_dst++) = *(cur_src++);
	}

	return dest;
}

