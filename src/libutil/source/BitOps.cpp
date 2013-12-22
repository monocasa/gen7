#include "util/BitOps.h"

#include <cstdint>

namespace util {

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

} //namespace util

