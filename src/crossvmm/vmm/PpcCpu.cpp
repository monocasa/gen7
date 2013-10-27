#include "MemoryManager.h"
#include "PpcCpu.h"
#include "XenonRealMemory.h"

#include "shared/Util.h"

#include <cstdio>

void PpcCpu::MmuContext::Init()
{
	for( int i = 0; i < NUM_HVREALMODE_PML3S; i++ ) {
		hvRealModePml3s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	}

	ir32Pml3 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

	for( int i = 0; i < NUM_IR32_PML2S; i++ ) {
		ir32Pml2s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	}

	for( int i = 0; i < NUM_IR32_PML1S; i++ ) {
		ir32Pml1s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	}
}

void PpcCpu::MmuContext::MapFull()
{
	hvRealModePml3s[ 0 ][ 0 ] = (((uint64_t)xenonReal.GetRamPml2(0)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 0 ][ 1 ] = (((uint64_t)xenonReal.GetRamPml2(1)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 0 ][ 2 ] = (((uint64_t)xenonReal.GetRamPml2(2)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 0 ][ 3 ] = (((uint64_t)xenonReal.GetRamPml2(3)) - 0xFFFFFFFF80000000UL) | 1;

	hvRealModePml3s[ 1 ][ 0 ] = (((uint64_t)xenonReal.GetRamPml2(0)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 1 ][ 1 ] = (((uint64_t)xenonReal.GetRamPml2(1)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 1 ][ 2 ] = (((uint64_t)xenonReal.GetRamPml2(2)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 1 ][ 3 ] = (((uint64_t)xenonReal.GetRamPml2(3)) - 0xFFFFFFFF80000000UL) | 1;

	hvRealModePml3s[ 2 ][ 0 ] = (((uint64_t)xenonReal.GetSocPml2(0)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 2 ][ 1 ] = (((uint64_t)xenonReal.GetSocPml2(1)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 2 ][ 2 ] = (((uint64_t)xenonReal.GetSocPml2(2)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 2 ][ 3 ] = (((uint64_t)xenonReal.GetSocPml2(3)) - 0xFFFFFFFF80000000UL) | 1;

	hvRealModePml3s[ 3 ][ 0 ] = (((uint64_t)xenonReal.GetRamPml2(0)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 3 ][ 1 ] = (((uint64_t)xenonReal.GetRamPml2(1)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 3 ][ 2 ] = (((uint64_t)xenonReal.GetRamPml2(2)) - 0xFFFFFFFF80000000UL) | 1;
	hvRealModePml3s[ 3 ][ 3 ] = (((uint64_t)xenonReal.GetRamPml2(3)) - 0xFFFFFFFF80000000UL) | 1;

	mm.SetLowerPml3( hvRealModePml3s[ 0 ], 0x0000000000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 1 ], 0x0000010000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 2 ], 0x0000020000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 3 ], 0x0000030000000000UL );
}

void PpcCpu::DumpContext()
{
	printf( "  r0 %16lx |   r1 %16lx |   r2 %16lx |   r3 %16lx\n", context.gpr[ 0], context.gpr[ 1], context.gpr[ 2], context.gpr[ 3] );
	printf( "  r4 %16lx |   r5 %16lx |   r6 %16lx |   r7 %16lx\n", context.gpr[ 4], context.gpr[ 5], context.gpr[ 6], context.gpr[ 7] );
	printf( "  r8 %16lx |   r9 %16lx |  r10 %16lx |  r11 %16lx\n", context.gpr[ 8], context.gpr[ 9], context.gpr[10], context.gpr[11] );
	printf( " r12 %16lx |  r13 %16lx |  r14 %16lx |  r15 %16lx\n", context.gpr[12], context.gpr[13], context.gpr[14], context.gpr[15] );
	printf( " r16 %16lx |  r17 %16lx |  r18 %16lx |  r19 %16lx\n", context.gpr[16], context.gpr[17], context.gpr[18], context.gpr[19] );
	printf( " r20 %16lx |  r21 %16lx |  r22 %16lx |  r23 %16lx\n", context.gpr[20], context.gpr[21], context.gpr[22], context.gpr[23] );
	printf( " r24 %16lx |  r25 %16lx |  r26 %16lx |  r27 %16lx\n", context.gpr[24], context.gpr[25], context.gpr[26], context.gpr[27] );
	printf( " r28 %16lx |  r29 %16lx |  r30 %16lx |  r31 %16lx\n", context.gpr[28], context.gpr[29], context.gpr[30], context.gpr[31] );
	printf( "  pc %16lx |  ctr %16lx\n", context.pc, context.ctr );
	printf( "hid6 %16lx\n", context.hid6 );
}

void PpcCpu::Init()
{
	mmuContext.Init();
	mmuContext.MapFull();
}

#define PC ((uint32_t*)context.pc)

#define D_SI(x) (Util::SignExtend<int32_t,16>( x & 0x0000FFFF ))
#define D_UI(x) (x & 0x0000FFFF)
#define D_RA(x) ((x >> 16) & 0x1F)
#define D_RT(x) ((x >> 21) & 0x1F)
#define D_RS(x) ((x >> 21) & 0x1F)

#define MD_RA(x) ((x >> 16) & 0x1F)
#define MD_RS(x) ((x >> 21) & 0x1F)

#define X_RC(x) (x & 1)
#define X_XO(x) ((x >> 1) & 0x3FF)
#define X_RB(x) ((x >> 11) & 0x1F)
#define X_RA(x) ((x >> 16) & 0x1F)
#define X_RS(x) ((x >> 21) & 0x1F)

#define XFX_SPR(x) (((x >> 16) & 0x1F) | ((x >> 6) & 0x3E0))
#define XFX_RS(x)  ((x >> 21) & 0x1F)

InterInstr PpcCpu::BuildIntermediateTable19( const uint32_t nativeInstr, uint64_t pc )
{
	const int xo = X_XO(nativeInstr);

	switch( xo ) {
		case 150: { //isync
			return InterInstr( InstrOp::NOP );
		}

		default: {
			return InterInstr( InstrOp::UNKNOWN_OPCODE, xo + 1900000, nativeInstr, pc );
		}
	}
}

InterInstr PpcCpu::BuildIntermediateSpecial( const uint32_t nativeInstr, uint64_t pc )
{
	const int xo = X_XO(nativeInstr);

	switch( xo ) {
		case 60: { //andc
			if( X_RC(nativeInstr) ) {
				return InterInstr( InstrOp::UNKNOWN_OPCODE, xo + 3100000, nativeInstr, pc );
			}
			const int rs = X_RS(nativeInstr);
			const int ra = X_RA(nativeInstr);
			const int rb = X_RB(nativeInstr);

			return InterInstr( InstrOp::ANDC, rs, rb, ra );
		}

		case 339: { //mfspr
			const int spr = XFX_SPR(nativeInstr);
			const int destReg = XFX_RS(nativeInstr);

			return InterInstr( InstrOp::READ_SYS, destReg, spr );
		}

		case 444: { //or
			if( X_RC(nativeInstr) ) {
				return InterInstr( InstrOp::UNKNOWN_OPCODE, xo + 3100000, nativeInstr, pc );
			}
			const int rs = X_RS(nativeInstr);
			const int ra = X_RA(nativeInstr);
			const int rb = X_RB(nativeInstr);

			return InterInstr( InstrOp::OR, rs, rb, ra );
		}

		case 467: { //mtspr
			const int spr = XFX_SPR(nativeInstr);
			const int sourceReg = XFX_RS(nativeInstr);

			switch( spr ) {
				case 9: { //ctr
					return InterInstr( InstrOp::MOVE_REG, sourceReg, 32 );
				}
				default: {
					return InterInstr( InstrOp::SET_SYS_REG, sourceReg, spr );
				}
			}
		}

		case 498: { //slbia
			return InterInstr( InstrOp::PPC_SLBIA );
		}

		case 598: { //sync
			return InterInstr( InstrOp::NOP );
		}

		default: {
			return InterInstr( InstrOp::UNKNOWN_OPCODE, xo + 3100000, nativeInstr, pc );
		}
	}
}

InterInstr PpcCpu::BuildIntermediate( const uint32_t nativeInstr, uint64_t pc )
{
	const int opcode = nativeInstr >> 26;

	switch( opcode ) {
		case 14: { //addi
			if( D_RA(nativeInstr) != 0 ) {
				return InterInstr( InstrOp::UNKNOWN_OPCODE, opcode, nativeInstr, pc );
			}

			int64_t imm = D_SI(nativeInstr);
			int rt = D_RT(nativeInstr);
			return InterInstr( InstrOp::LOAD_IMM, rt, imm );
		}

		case 15: { //addis
			if( D_RA(nativeInstr) != 0 ) {
				return InterInstr( InstrOp::UNKNOWN_OPCODE, opcode, nativeInstr, pc );
			}
			int64_t imm = D_SI(nativeInstr) << 16;
			int rt = D_RT(nativeInstr);

			return InterInstr( InstrOp::LOAD_IMM, rt, imm );
		}
			
		case 18: { //branch
			if( nativeInstr & 3 ) {
				return InterInstr( InstrOp::UNKNOWN_OPCODE, opcode, nativeInstr, pc );
			}
			int offset = Util::SignExtend<int,26>( nativeInstr & 0x03FFFFFC );
			uint64_t target = offset + context.pc;

			return InterInstr( InstrOp::BRANCH_ALWAYS, target );
		}

		case 19: { //table 19
			return BuildIntermediateTable19( nativeInstr, pc );
		}

		case 24: { //ori
			const int rs = D_RS(nativeInstr);
			const int ra = D_RA(nativeInstr);
			const uint64_t imm = D_UI(nativeInstr);
			if( rs == 0 && ra == 0 && imm == 0 ) {
				return InterInstr( InstrOp::NOP );
			}
			return InterInstr( InstrOp::OR_IMM, rs, ra, imm );
		}

		case 30: { //rotate
			const int rs = D_RS(nativeInstr);
			const int ra = D_RA(nativeInstr);
			if( (nativeInstr & 0xFFFF) == 0x07C6 ) {
				return InterInstr( InstrOp::SLL64, rs, ra, 32 );
			}
			else if( (nativeInstr & 0xFFFF) == 0x64c6 ) {
				return InterInstr( InstrOp::SLL64, rs, ra, 44 );
			}
			return InterInstr( InstrOp::UNKNOWN_OPCODE, opcode, nativeInstr, pc );
		}

		case 31: { //special
			return BuildIntermediateSpecial( nativeInstr, pc );
		} 	

		default: {
			return InterInstr( InstrOp::UNKNOWN_OPCODE, opcode, nativeInstr, pc );
		}
	}
}

bool PpcCpu::SetSystemReg( int sysReg, uint64_t value )
{
	switch( sysReg ) {
		case SPR_HID6: {
			printf( "PpcCpu:  HID6 set to %016lx\n", value );
			context.hid6 = value;
			return true;
		}

		default: {
			printf( "Unknown Ppc System Reg %d set to %016lx\n", sysReg, value );
			return false;
		}
	}
}

bool PpcCpu::ReadSystemReg( int sysReg, uint64_t &value )
{
	switch( sysReg ) {
		case SPR_HID6: {
			value = context.hid6;
			return true;
		}

		default: {
			printf( "Unknown Ppc System Reg %d read from\n", sysReg );
			return false;
		}
	}
}

bool PpcCpu::InterpretProcessorSpecific( InterInstr &instr )
{
	switch( instr.op ) {
		case PPC_SLBIA: {
			printf( "SLBIA\n" );
			return true;
		}

		default: {
			printf( "Unimplemented PowerPC specific op %d\n", instr.op );
			return false;
		}
	}
}

void PpcCpu::Execute()
{
	bool running = true;

	while( running ) {
		const uint32_t instruction = *PC;

		InterInstr intermediate = BuildIntermediate( instruction, context.pc );

		context.pc += sizeof(uint32_t);

		if( !InterpretIntermediate( intermediate ) ) {
			DumpContext();
			break;
		}
	}
}

