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

#define B_LK(x) (x & 1)
#define B_AA(x) ((x >> 1) & 1)
#define B_BD(x) (Util::SignExtend<int64_t,16>( x & 0x0000FFFC ))
#define B_BI(x) ((x >> 16) & 0x1F)
#define B_BO(x) ((x >> 21) & 0x1F)

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

#define XO_RC(x) (x & 1)
#define XO_RB(x) ((x >> 11) & 0x1F)
#define XO_RA(x) ((x >> 16) & 0x1F)
#define XO_RT(x) ((x >> 21) & 0x1F)

int PpcCpu::BuildIntermediateBranchConditional( InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc )
{
	if( B_LK(nativeInstr) || B_AA(nativeInstr) ) {
		intermediates[0].BuildUnknown( 16, nativeInstr, pc );
		return 1;
	}

	if( B_BI(nativeInstr) ) {
		intermediates[0].BuildUnknown( 16, nativeInstr, pc );
		return 1;
	}

	const uint64_t target = pc + B_BD( nativeInstr );

	switch( B_BO(nativeInstr) ) {
		case 25: { //bdnz+
			intermediates[0].BuildSubuImm( GPR_CTR, GPR_CTR, 1 );
			intermediates[1].BuildBranchGprNotZero( GPR_CTR, target );
			return 2;
		}

		default: {
			printf( "BO=%d BI=%d\n", B_BO(nativeInstr), B_BI(nativeInstr) );
			intermediates[0].BuildUnknown( 16, nativeInstr, pc );
			return 1;
		}
	}
}

int PpcCpu::BuildIntermediateTable19( InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc )
{
	const int xo = X_XO(nativeInstr);

	switch( xo ) {
		case 150: { //isync
			intermediates[0].BuildNop();
			return 1;
		}

		default: {
			intermediates[0].BuildUnknown( xo + 1900000, nativeInstr, pc );
			return 1;
		}
	}
}

int PpcCpu::BuildIntermediateSpecial( InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc )
{
	const int xo = X_XO(nativeInstr);

	switch( xo ) {
		case 60: { //andc
			if( X_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}
			const int rs = X_RS(nativeInstr);
			const int ra = X_RA(nativeInstr);
			const int rb = X_RB(nativeInstr);

			intermediates[0].BuildAndc( rs, rb, ra );
			return 1;
		}

		case 266: { //add
			if( XO_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}
			const int rt = XO_RT(nativeInstr);
			const int ra = XO_RA(nativeInstr);
			const int rb = XO_RB(nativeInstr);

			intermediates[0].BuildAdd( ra, rb, rt );
			return 1;
		}

		case 274: { //tlbiel
			const int rb = X_RB(nativeInstr);

			intermediates[0].BuildPpcTlbiel( rb );
			return 1;
		}

		case 339: { //mfspr
			const int spr = XFX_SPR(nativeInstr);
			const int destReg = XFX_RS(nativeInstr);

			intermediates[0].BuildReadSystem( destReg, spr );
			return 1;
		}

		case 402: { //slbmte
			const int rs = X_RS(nativeInstr);
			const int rb = X_RB(nativeInstr);

			intermediates[0].BuildPpcSlbmte( rs, rb );
			return 1;
		}

		case 444: { //or
			if( X_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}
			const int rs = X_RS(nativeInstr);
			const int ra = X_RA(nativeInstr);
			const int rb = X_RB(nativeInstr);

			intermediates[0].BuildOr( rs, rb, ra );
			return 1;
		}

		case 467: { //mtspr
			const int spr = XFX_SPR(nativeInstr);
			const int sourceReg = XFX_RS(nativeInstr);

			switch( spr ) {
				case SPR_CTR: {
					intermediates[0].BuildMoveReg( sourceReg, GPR_CTR );
					break;
				}
				default: {
					intermediates[0].BuildSetSystemReg( sourceReg, spr );
					break;
				}
			}

			return 1;
		}

		case 498: { //slbia
			intermediates[0].BuildPpcSlbia();
			return 1;
		}

		case 598: { //sync
			intermediates[0].BuildNop();
			return 1;
		}

		default: {
			intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
			return 1;
		}
	}
}

int PpcCpu::BuildIntermediate( InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc )
{
	const int opcode = nativeInstr >> 26;

	switch( opcode ) {
		case 14: { //addi
			if( D_RA(nativeInstr) != 0 ) {
				intermediates[0].BuildUnknown( opcode, nativeInstr, pc );
				return 1;
			}

			int64_t imm = D_SI(nativeInstr);
			int rt = D_RT(nativeInstr);

			intermediates[0].BuildLoadImm( rt, imm );
			return 1;
		}

		case 15: { //addis
			if( D_RA(nativeInstr) != 0 ) {
				intermediates[0].BuildUnknown( opcode, nativeInstr, pc );
				return 1;
			}

			int64_t imm = D_SI(nativeInstr) << 16;
			int rt = D_RT(nativeInstr);

			intermediates[0].BuildLoadImm( rt, imm );
			return 1;
		}

		case 16: { //branch conditional
			return BuildIntermediateBranchConditional( intermediates, nativeInstr, pc );
		}

		case 18: { //branch
			if( nativeInstr & 3 ) {
				intermediates[0].BuildUnknown( opcode, nativeInstr, pc );
				return 1;
			}
			int offset = Util::SignExtend<int,26>( nativeInstr & 0x03FFFFFC );
			uint64_t target = offset + context.pc;

			intermediates[0].BuildBranchAlways( target );
			return 1;
		}

		case 19: { //table 19
			return BuildIntermediateTable19( intermediates, nativeInstr, pc );
		}

		case 24: { //ori
			const int rs = D_RS(nativeInstr);
			const int ra = D_RA(nativeInstr);
			const uint64_t imm = D_UI(nativeInstr);

			if( rs == 0 && ra == 0 && imm == 0 ) {
				intermediates[0].BuildNop();
				return 1;
			}

			intermediates[0].BuildOrImm( rs, ra, imm );
			return 1;
		}

		case 30: { //rotate
			const int rs = D_RS(nativeInstr);
			const int ra = D_RA(nativeInstr);

			if( (nativeInstr & 0xFFFF) == 0x07C6 ) {
				intermediates[0].BuildSll64( rs, ra, 32 );
			}
			else if( (nativeInstr & 0xFFFF) == 0x64c6 ) {
				intermediates[0].BuildSll64( rs, ra, 44 );
			}
			else {
				intermediates[0].BuildUnknown( opcode, nativeInstr, pc );
			}
			return 1;
		}

		case 31: { //special
			return BuildIntermediateSpecial( intermediates, nativeInstr, pc );
		} 	

		default: {
			intermediates[0].BuildUnknown( opcode, nativeInstr, pc );
			return 1;
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

		case PPC_SLBMTE: {
			const int rs = instr.args[0];
			const int rb = instr.args[1];

			printf( "SLBMTE RS=%016lx RB=%016lx\n", ReadGPR(rs), ReadGPR(rb) );
			return true;
		}

		case PPC_TLBIEL: {
			const int rb = instr.args[0];

			printf( "TLBIEL RB=%016lx\n", ReadGPR(rb) );
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

	InterInstr intermediates[ 10 ];

	while( running ) {
		const uint32_t instruction = *PC;

		int numIntermediates = BuildIntermediate( intermediates, instruction, context.pc );

		context.pc += sizeof(uint32_t);

		for( int i = 0; i < numIntermediates; i++ ) {
			if( !InterpretIntermediate( intermediates[i] ) ) {
				DumpContext();
				return;
			}
		}
	}
}

