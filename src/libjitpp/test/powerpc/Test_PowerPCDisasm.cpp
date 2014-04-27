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

void testDecode( const uint32_t opcode, const char *expectedString )
{
	testDecode( opcode, 0x00000000, expectedString );
}

TEST(PowerPCDisasm, Unknown)
{
	testDecode( 0x00000000, "<UNKNOWN_00000000> OPCD=0" );
}

TEST(PowerPCDisasm, Addi)
{
	// Test negative and sign extension
	testDecode( 0x3821fffc, "addi     r1, r1, -4" );

	// Test li pseudo op
	testDecode( 0x3be00000, "li       r31, 0" );
}

TEST(PowerPCDisasm, Addis)
{
	testDecode( 0x3c600000, "lis      r3, 0" );
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

TEST(PowerPCDisasm, Cmplwi)
{
	testDecode( 0x28030000, "cmplwi   r3, 0x0000" );
}

TEST(PowerPCDisasm, Ori)
{
	testDecode( 0x60630000, "ori      r3, r3, 0x0000" );
}

