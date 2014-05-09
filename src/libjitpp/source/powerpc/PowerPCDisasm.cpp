#include "jitpp/powerpc/PowerPCDisasm.h"

#include "util/Compilers.h"

#include <cstdio>

namespace jitpp {

void PowerPCDisasm::OnUnknownInstruction( uint32_t instr, UnknownCode code, 
	                                      int codeArg, uint64_t pc )
{
	UNUSED( pc );

	const char * codeStr;

	switch( code ) {
		case UnknownCode::OPCD:             codeStr = "OPCD";             break;
		case UnknownCode::SPECIAL:          codeStr = "SPECIAL";          break;
		case UnknownCode::UNKNOWN_SPR_READ: codeStr = "UNKNOWN_SPR_READ"; break;
	}

	sprintf( buffer, "<UNKNOWN_%08x> %s=%d", instr, codeStr, codeArg );
}

void PowerPCDisasm::OnAddi( int rt, int ra, int16_t si )
{
	sprintf( buffer, "addi     r%d, r%d, %d", rt, ra, si );
}

void PowerPCDisasm::OnAddis( int rt, int ra, int16_t si )
{
	sprintf( buffer, "addis    r%d, r%d, %d", rt, ra, si );
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

void PowerPCDisasm::OnCmpldi( int bf, int ra, uint16_t ui )
{
	if( 0 == bf ) {
		sprintf( buffer, "cmpldi   r%d, 0x%04x", ra, ui );
	}
	else {
		sprintf( buffer, "cmpldi   cr%d, r%d, 0x%04x", bf, ra, ui );
	}
}

void PowerPCDisasm::OnCmplwi( int bf, int ra, uint16_t ui )
{
	if( 0 == bf ) {
		sprintf( buffer, "cmplwi   r%d, 0x%04x", ra, ui );
	}
	else {
		sprintf( buffer, "cmplwi   cr%d, r%d, 0x%04x", bf, ra, ui );
	}
}

void PowerPCDisasm::OnLi( int rt, int16_t si )
{
	sprintf( buffer, "li       r%d, %d", rt, si );
}

void PowerPCDisasm::OnLis( int rt, int16_t si )
{
	sprintf( buffer, "lis      r%d, %d", rt, si );
}

void PowerPCDisasm::OnMflr( int rt )
{
	sprintf( buffer, "mflr     r%d", rt );
}

void PowerPCDisasm::OnOri( int ra, int rs, uint16_t ui )
{
	sprintf( buffer, "ori      r%d, r%d, 0x%04x", ra, rs, ui );
}

void PowerPCDisasm::OnOris( int ra, int rs, uint16_t ui )
{
	sprintf( buffer, "oris     r%d, r%d, 0x%04x", ra, rs, ui );
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

