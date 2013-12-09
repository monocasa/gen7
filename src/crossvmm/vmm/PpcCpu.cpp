#include "MemoryManager.h"
#include "PpcCpu.h"
#include "XenonRealMemory.h"

#include "util/BitOps.h"

#include <cstdio>

extern "C" void hyper_quit();

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

	ir32Pml3[ 0 ] = (((uint64_t)&ir32Pml2s[ 0 ]) - 0xFFFFFFFF80000000) | 1;
	ir32Pml3[ 1 ] = (((uint64_t)&ir32Pml2s[ 1 ]) - 0xFFFFFFFF80000000) | 1;
	ir32Pml3[ 2 ] = (((uint64_t)&ir32Pml2s[ 2 ]) - 0xFFFFFFFF80000000) | 1;
	ir32Pml3[ 3 ] = (((uint64_t)&ir32Pml2s[ 3 ]) - 0xFFFFFFFF80000000) | 1;

	DisableRelocation();
}

void PpcCpu::MmuContext::DisableRelocation()
{
	mm.SetLowerPml3( hvRealModePml3s[ 0 ], 0x0000000000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 1 ], 0x0000010000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 2 ], 0x0000020000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 3 ], 0x0000030000000000UL );
}

void PpcCpu::MmuContext::EnableRelocation()
{
	mm.SetLowerPml3( ir32Pml3, 0x0000000000000000UL );
	//mm.ClearLowerPml3( 0x0000010000000000UL );
	//mm.ClearLowerPml3( 0x0000020000000000UL );
	//mm.ClearLowerPml3( 0x0000030000000000UL );
}

bool PpcCpu::MmuContext::IsInstructionMapped( uint64_t /*addr*/ )
{
	return !((context.msr & 0x30) != 0);
}

int PpcCpu::MmuContext::GetTlbHint() {
	int suggested = (nextTlbHint++) & (NUM_TLB_ENTRIES - 1);
	int index = (suggested >> 2) & 0xFF;
	int set = 8 >> (suggested & 3);

	return (index << 4) | set;
}

void PpcCpu::MmuContext::WriteTlbVpn( uint64_t value )
{
	uint64_t avpn = value & 0xFFFFFFFFFFFFFF80UL;
	avpn <<= 16;
	printf( "tlbIndex[%d] avpn=%016lx valid=%s\n", currentTlbIndex, avpn, (value&1)?"true":"false" );
}

void PpcCpu::MmuContext::WriteTlbRpn( uint64_t value )
{
	uint64_t rpn = value & 0xFFFFFFFFFFFF0000UL;
	printf( "tlbIndex[%d] rpn=%016lx\n", currentTlbIndex, rpn );
}

void PpcCpu::DumpContext()
{
	printf( "   r0 %16lx |    r1 %16lx |    r2 %16lx |    r3 %16lx\n", context.gpr[ 0], context.gpr[ 1], context.gpr[ 2], context.gpr[ 3] );
	printf( "   r4 %16lx |    r5 %16lx |    r6 %16lx |    r7 %16lx\n", context.gpr[ 4], context.gpr[ 5], context.gpr[ 6], context.gpr[ 7] );
	printf( "   r8 %16lx |    r9 %16lx |   r10 %16lx |   r11 %16lx\n", context.gpr[ 8], context.gpr[ 9], context.gpr[10], context.gpr[11] );
	printf( "  r12 %16lx |   r13 %16lx |   r14 %16lx |   r15 %16lx\n", context.gpr[12], context.gpr[13], context.gpr[14], context.gpr[15] );
	printf( "  r16 %16lx |   r17 %16lx |   r18 %16lx |   r19 %16lx\n", context.gpr[16], context.gpr[17], context.gpr[18], context.gpr[19] );
	printf( "  r20 %16lx |   r21 %16lx |   r22 %16lx |   r23 %16lx\n", context.gpr[20], context.gpr[21], context.gpr[22], context.gpr[23] );
	printf( "  r24 %16lx |   r25 %16lx |   r26 %16lx |   r27 %16lx\n", context.gpr[24], context.gpr[25], context.gpr[26], context.gpr[27] );
	printf( "  r28 %16lx |   r29 %16lx |   r30 %16lx |   r31 %16lx\n", context.gpr[28], context.gpr[29], context.gpr[30], context.gpr[31] );
	printf( "   pc %16lx |   msr %16lx |   ctr %16lx |    lr %16lx\n", context.pc, context.msr, context.ctr, context.lr );
	printf( "   cr         %08x |   xer         %8x\n", context.cr, context.xer );
	printf( " srr0 %16lx |  srr1 %16lx | hrmor %16lx |  hid6 %16lx\n", context.srr0, context.srr1, context.hrmor, context.hid6 );
	printf( "lpidr %16lx |  lpcr %16lx\n", context.lpidr, context.lpcr );
	printf( "sprg0 %16lx | sprg1 %16lx | sprg2 %16lx | sprg3 %16lx\n", context.sprg0, context.sprg1, context.sprg2, context.sprg3 );
}

void PpcCpu::SetMsr( uint64_t newMsr )
{
	switch( newMsr & 0x30 ) {
		case 0x00: {
			mmuContext.DisableRelocation();
			break;
		}

		case 0x30: {
			mmuContext.EnableRelocation();
			break;
		}

		default: {
			printf( "msr set to %08lx.  Relocations don't match\n", context.msr );
			hyper_quit();
			break;
		}
	}

	context.msr = newMsr;
}

void PpcCpu::Init()
{
	mmuContext.Init();
	mmuContext.MapFull();
}

#define PC ((uint32_t*)context.pc)

void PpcCpu::SetPC( uint64_t pc )
{
	if( !context.Is64Bit() ) {
		pc = (uint32_t)pc;
	}

	context.pc = pc;
}

bool PpcCpu::SetSystemReg( int sysReg, uint64_t value )
{
	switch( sysReg ) {
		case jit::PowerPCHelpers::SPR_SRR0: {
			context.srr0 = value;
			return true;
		}

		case jit::PowerPCHelpers::SPR_SRR1: {
			context.srr1 = value;
			return true;
		}

		case jit::PowerPCHelpers::SPR_SPRG1: {
			context.sprg1 = value;
			return true;
		}

		case jit::PowerPCHelpers::SPR_HRMOR: {
			printf( "PpcCpu:  HRMOR set to %016lx\n", value );
			context.hrmor = value & 0x000003FFFFF00000;
			return true;
		}

		case jit::PowerPCHelpers::SPR_LPCR: {
			printf( "PpcCpu:  LPCR set to %016lx\n", value );
			context.lpcr = value;
			return true;
		}

		case jit::PowerPCHelpers::SPR_LPIDR: {
			printf( "PpcCpu:  LPIDR set to %016lx\n", value );
			context.lpidr = value;
			return true;
		}

		case jit::PowerPCHelpers::SPR_PPE_TLB_INDEX: {
			printf( "PpcCpu:  PPE_TLB_Index set to %016lx\n", value );
			mmuContext.WriteTlbIndex( value );
			return true;
		}

		case jit::PowerPCHelpers::SPR_PPE_TLB_VPN: {
			printf( "PpcCpu:  PPE_TLB_Vpn set to %016lx\n", value );
			mmuContext.WriteTlbVpn( value );
			return true;
		}

		case jit::PowerPCHelpers::SPR_PPE_TLB_RPN: {
			printf( "PpcCpu:  PPE_TLB_Rpn set to %016lx\n", value );
			mmuContext.WriteTlbRpn( value );
			return true;
		}

		case jit::PowerPCHelpers::SPR_HID6: {
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
		case jit::PowerPCHelpers::SPR_XER: {
			value = context.xer;
			return true;
		}

		case jit::PowerPCHelpers::SPR_SRR0: {
			value = context.srr0;
			return true;
		}

		case jit::PowerPCHelpers::SPR_LPCR: {
			value = context.lpcr;
			return true;
		}

		case jit::PowerPCHelpers::SPR_PPE_TLB_INDEX_HINT: {
			value = mmuContext.GetTlbHint();
			return true;
		}

		case jit::PowerPCHelpers::SPR_HID6: {
			value = context.hid6;
			return true;
		}

		case jit::PowerPCHelpers::SPR_PIR: {
			value = context.coreNum;
			return true;
		}

		case jit::PowerPCHelpers::SPR_MSR: {
			value = context.msr;
			return true;
		}

		default: {
			printf( "Unknown Ppc System Reg %d read from\n", sysReg );
			return false;
		}
	}
}

bool PpcCpu::InterpretProcessorSpecific( jit::InterInstr &instr )
{
	switch( instr.op ) {
		case jit::PPC_SLBIA: {
			printf( "SLBIA\n" );
			return true;
		}

		case jit::PPC_SLBMTE: {
			const int rs = instr.args[0];
			const int rb = instr.args[1];

			printf( "SLBMTE RS=%016lx RB=%016lx\n", ReadGPR64(rs), ReadGPR64(rb) );
			return true;
		}

		case jit::PPC_TLBIEL: {
			const int rb = instr.args[0];
			const bool l = instr.args[1];

			printf( "TLBIEL RB=%016lx L=%d\n", ReadGPR64(rb), l );
			return true;
		}

		case jit::PPC_STWCX: {
			const int addrReg = instr.args[0];
			const int offsetReg = instr.args[1];
			const int valueReg = instr.args[2];

			uint64_t addr = ReadGPR64( addrReg );
			if( offsetReg != 0 ) {
				addr += ReadGPR64( offsetReg );
			}

			context.ClearCr( 0 );

			if( isReserved && reservation == addr ) {
				WriteMem32( addr, ReadGPR64(valueReg) );

				isReserved = false;

				context.SetCrEq( 0 );
			}

			return true;
		}

		case jit::PPC_CMPLDI: {
			const int ra = instr.args[0];
			const int cr = instr.args[1];
			const uint32_t imm = instr.args[2];

			const uint64_t value = ReadGPR64( ra );

			context.ClearCr( cr );

			if( value < imm ) {
				context.SetCrLt( cr );
			}
			else if( value > imm ) {
				context.SetCrGt( cr );
			}
			else {
				context.SetCrEq( cr );
			}

			//TODO:  Here we need to set cr so

			return true;
		}

		case jit::PPC_CMPLWI: {
			const int ra = instr.args[0];
			const int cr = instr.args[1];
			const uint32_t imm = instr.args[2];

			const uint64_t value = ReadGPR32( ra );

			context.ClearCr( cr );

			if( value < imm ) {
				context.SetCrLt( cr );
			}
			else if( value > imm ) {
				context.SetCrGt( cr );
			}
			else {
				context.SetCrEq( cr );
			}

			//TODO:  Here we need to set cr so

			return true;
		}

		case jit::PPC_RFID: {
			SetMsr( context.srr1 );
			SetPC( context.srr0 );

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

	jit::InterInstr intermediates[ 10 ];

	while( running ) {
		if( context.pc == 0x80006aec) {
			context.pc = 0x8FFF6aec;
		}

		if( !mmuContext.IsInstructionMapped( context.pc ) ) {
			printf( "PpcCpu:  ISI @ %08lx\n", context.pc );
			context.srr0 = context.pc;
			context.srr1 = context.msr;
			SetMsr( context.msr & ~0x30 );
			SetPC( 0x400 );
		}

		const uint32_t instruction = *PC;

		//DumpContext();
		//printf( "%08lx : %04x\n", context.pc, instruction );

		int numIntermediates = BuildIntermediate( intermediates, instruction, context.pc );

		context.pc += sizeof(uint32_t);

		for( int i = 0; i < numIntermediates; i++ ) {
			if( !InterpretIntermediate( intermediates[i] ) ) {

				printf( "Intermediate interpretation failed:\n\t%s\n", GetErrorString() );
				context.pc -= sizeof(uint32_t);

				DumpContext();
				return;
			}
		}
	}
}

