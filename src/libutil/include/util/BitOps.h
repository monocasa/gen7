#ifndef GEN7_LIBUTIL_BITOPS_H
#define GEN7_LIBUTIL_BITOPS_H

#include <cstdint>

namespace util {

template<typename T> static inline T ByteSwap( T value );

template<>
uint16_t ByteSwap<uint16_t>( uint16_t value )
{
	return (value >> 8) | (value << 8);
}

template<>
uint32_t ByteSwap<uint32_t>( uint32_t value )
{
	return ((uint32_t)ByteSwap<uint16_t>( value ) << 16) | ByteSwap<uint16_t>( value >> 16 );
}

template<>
uint64_t ByteSwap<uint64_t>( uint64_t value )
{
	return ((uint64_t)ByteSwap<uint32_t>( value ) << 32) | ByteSwap<uint32_t>( value >> 32 );
}

template<typename T, unsigned B>
static inline T SignExtend( const T x )
{
	struct {
		T x:B;
	} s;

	return s.x = x;
}

} //namespace util

#endif //GEN7_LIBUTIL_BITOPS_H

