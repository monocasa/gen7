#include "jit/powerpc/PowerPCDisasm.h"

#include <gtest/gtest.h>

#include <cstdint>

void testDecode( const uint32_t opcode, const uint32_t pc, const char *expectedString )
{
	jit::PowerPCDisasm disasm;
	uint8_t instrBuffer[ jit::Disasm::INSTR_BUFFER_SIZE ];
	char string[ 512 ];
	char opcodeString[ 20 ];

	instrBuffer[0] = opcode;
	instrBuffer[1] = opcode >> 8;
	instrBuffer[2] = opcode >> 16;
	instrBuffer[3] = opcode >> 24;

	disasm.Disassemble( instrBuffer, pc, string );

	sprintf( opcodeString, "%08x : %08x", pc, opcode );

	ASSERT_STREQ( expectedString, string ) << opcodeString;
}

TEST(PowerPCDisasm, Unknown)
{
	testDecode( 0x00000000, 0x00000000, "<UNKNOWN_00000000> OPCOD=0" );
}

TEST(PowerPCDisasm, Branch)
{
	// Test normal use case
	testDecode( 0x48000100, 0x00001000, "b\tloc_1100" );
	testDecode( 0x48000101, 0x00001000, "bl\tloc_1100" );

	// Test branch absolute
	testDecode( 0x48000102, 0x00001000, "ba\tloc_100" );
	testDecode( 0x48000103, 0x00001000, "bla\tloc_100" );

	// Test sign extension
	testDecode( 0x4BFFFFFC, 0x00002000, "b\tloc_1ffc" );
}

