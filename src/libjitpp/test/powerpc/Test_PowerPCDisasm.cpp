#include "jitpp/powerpc/PowerPCDisasm.h"

#include <gtest/gtest.h>

#include <cstdint>

void testDecode( const uint32_t opcode, const uint32_t pc, const char *expectedString )
{
	jitpp::PowerPCDisasm disasm;
	uint8_t instrBuffer[ jitpp::Disasm::INSTR_BUFFER_SIZE ];
	char string[ 512 ];
	char opcodeString[ 20 ];

	instrBuffer[0] = opcode;
	instrBuffer[1] = opcode >> 8;
	instrBuffer[2] = opcode >> 16;
	instrBuffer[3] = opcode >> 24;

	disasm.Disassemble( instrBuffer, pc, string );

	sprintf( opcodeString, "%08x : %08x", pc, opcode );

	EXPECT_STREQ( expectedString, string ) << opcodeString;
}

TEST(PowerPCDisasm, Unknown)
{
	testDecode( 0x00000000, 0x00000000, "<UNKNOWN_00000000> OPCOD=0" );
}

TEST(PowerPCDisasm, Addi)
{
	// Test negative and sign extension
	testDecode( 0x3821fffc, 0x00000000, "addi     r1, r1, -4" );

	// Test li pseudo op
	testDecode( 0x3be00000, 0x00000000, "li       r31, 0" );
}

TEST(PowerPCDisasm, Addis)
{
	testDecode( 0x3c600000, 0x00000000, "lis      r3, 0" );
}

TEST(PowerPCDisasm, Branch)
{
	// Test normal use case
	testDecode( 0x48000100, 0x00001000, "b        loc_1100" );
	testDecode( 0x48000101, 0x00001000, "bl       loc_1100" );

	// Test branch absolute
	testDecode( 0x48000102, 0x00001000, "ba       loc_100" );
	testDecode( 0x48000103, 0x00001000, "bla      loc_100" );

	// Test sign extension
	testDecode( 0x4BFFFFFC, 0x00002000, "b        loc_1ffc" );
}

