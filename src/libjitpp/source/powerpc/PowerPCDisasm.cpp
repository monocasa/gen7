#include "jitpp/powerpc/PowerPCDisasm.h"

#include "util/Compilers.h"

#include <cstdio>

namespace jitpp {

void PowerPCDisasm::OnUnknownInstruction( uint32_t instr, UnknownCode code, 
	                                      int codeArg, uint64_t pc )
{
	UNUSED( pc );

	switch( code ) {
		case UnknownCode::OPCD: {
			sprintf( buffer, "<UNKNOWN_%08x> OPCOD=%d", instr, codeArg );
		}
		break;
	}
}

void PowerPCDisasm::OnBranch( uint64_t target, bool link, bool abs )
{
	char opname[4];

	sprintf( opname, "b%s%s",
	         (link ? "l" : "" ),
	         (abs ? "a" : "" ) );

	sprintf( buffer, "%-8s loc_%lx",
	         opname,
	         target );
}

void PowerPCDisasm::Disassemble( uint8_t *instrBuffer, uint32_t pc, char *string )
{
	uint32_t instr;

	instr = (instrBuffer[0])
	      | (instrBuffer[1] << 8)
	      | (instrBuffer[2] << 16)
	      | (instrBuffer[3] << 24); 

	buffer = string;

	DecodeInstruction( instr, pc );
}

} //namespace jitpp

