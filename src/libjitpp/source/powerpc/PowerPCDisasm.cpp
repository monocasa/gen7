#include "jitpp/powerpc/PowerPCDisasm.h"
#include "jitpp/powerpc/PowerPCHelpers.h"

#include <cstdio>

namespace jitpp {

void PowerPCDisasm::DisassembleBranch( uint32_t instruction, uint32_t pc, char *string )
{
	const char *opcd;

	if( B_LK(instruction) && B_AA(instruction) ) {
		opcd = "bla";
	}
	else if( B_LK(instruction) ) {
		opcd = "bl";
	}
	else if( B_AA(instruction) ) {
		opcd = "ba";
	}
	else {
		opcd = "b";
	}

	uint64_t target = B_LI(instruction);

	if( !B_AA(instruction) ) {
		target += pc;
	}

	sprintf( string, "%s\tloc_%lx", opcd, target );
}

void PowerPCDisasm::Disassemble( uint8_t *instrBuffer, uint32_t pc, char *string )
{
	uint32_t instruction;

	instruction = (instrBuffer[0])
	            | (instrBuffer[1] << 8)
	            | (instrBuffer[2] << 16)
	            | (instrBuffer[3] << 24); 

	int opcode = OPCD( instruction );

	switch( opcode ) {
		case OPCD_BRANCH: {
			DisassembleBranch( instruction, pc, string );
			return;
		}

		default: {
			sprintf( string, "<UNKNOWN_%08x> OPCOD=%d", instruction, opcode );
			return;
		}
	}
}

} //namespace jitpp

