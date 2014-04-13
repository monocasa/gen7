#include "util/BitOps.h"

#include <gtest/gtest.h>

TEST(BitOps, ByteSwap16)
{
	EXPECT_EQ( 0x0123, util::ByteSwap<uint16_t>(0x2301) );
}

TEST(BitOps, ByteSwap32)
{
	EXPECT_EQ( 0x01234567, util::ByteSwap<uint32_t>(0x67452301) );
}

TEST(BitOps, ByteSwap64)
{
	EXPECT_EQ( 0x0123456789abcdef, util::ByteSwap<uint64_t>(0xefcdab8967452301) );
}

