#include "InterruptManager.h"
#include "MemoryManager.h"
#include "PpcCpu.h"
#include "XenonRealMemory.h"

#include "util/BitOps.h"

#include <cstdio>

extern "C" void hyper_quit();

void PpcCpu::MmuContext::SetNewVpn( uint64_t value, int index, int set )
{
	if( tlbEntries[index][set].IsValid() ) {
		printf( "rewriting valid tlb entry %d.%d\n", index, set );
		hyper_quit();
	}
	tlbEntries[index][set].vpn = value;

	if( tlbEntries[index][set].IsValid() ) {
		Map16MPage( tlbEntries[index][set].GetVpn(), tlbEntries[index][set].GetRpn() );
	}
	nextTlbHint++;
}

void PpcCpu::MmuContext::Map16MPage( uint64_t vpn, uint64_t rpn )
{
	printf( "mapping 16MiB page %08lx(%08lx)->%08lx\n", rpn, xenonReal.GetVmmPhysForXenonPhys(rpn), vpn );
	int pml2 = (vpn >> 30) & 0x3;
	int pmle = (vpn >> 21) & 0x1ff;

	uint64_t phys = xenonReal.GetVmmPhysForXenonPhys( rpn );

	for( int i = 0; i < 8; i++ ) {
		ir32Pml2s[pml2][pmle + i] = (phys + ((2 * 1024 * 1024UL) * i)) | 0x83;
	}
}

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

	ir32Pml3[ 0 ] = (((uint64_t)ir32Pml2s[ 0 ]) - 0xFFFFFFFF80000000) | 1;
	ir32Pml3[ 1 ] = (((uint64_t)ir32Pml2s[ 1 ]) - 0xFFFFFFFF80000000) | 1;
	ir32Pml3[ 2 ] = (((uint64_t)ir32Pml2s[ 2 ]) - 0xFFFFFFFF80000000) | 1;
	ir32Pml3[ 3 ] = (((uint64_t)ir32Pml2s[ 3 ]) - 0xFFFFFFFF80000000) | 1;

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

bool PpcCpu::MmuContext::IsInstructionMapped( uint64_t addr )
{
	if( context.msr & 0x30 ) {
		for( int i = 0 ; i < 256; i++ ) {
			for( int j = 0; j < 4; j++ ) {
				TlbEntry *entry = &tlbEntries[i][j];
				if( !entry->IsValid() ) {
					continue;
				}
				printf( "PpcCpu:  TLB Entry found vpn=%08lx rpn=%08lx addr=%08lx\n", entry->GetVpn(), entry->GetRpn(), addr );
				if( (addr >= entry->GetVpn()) && (addr < (entry->GetVpn() + (16 * 1024 * 1024))) ) {
					return true;
				}
			}
		}
		printf( "No tlb entry found for addr %08lx\n", addr );
		return false;
	}
	else {
		return true;
	}
}

int PpcCpu::MmuContext::GetTlbHint() {
	int suggested = (nextTlbHint) & (NUM_TLB_ENTRIES - 1);
	int index = (suggested >> 2) & 0xFF;
	int set = 8 >> (suggested & 3);

	return (index << 4) | set;
}

void PpcCpu::MmuContext::WriteTlbVpn( uint64_t value )
{
	uint64_t avpn = value & 0xFFFFFFFFFFFFFF80UL;
	avpn <<= 16;
	printf( "tlbIndex[%d] avpn=%016lx valid=%s\n", currentTlbIndex, avpn, (value&1)?"true":"false" );

	for( int i = 0; i < 4; i++ ) {
		const int flag = 0x8 >> i;
		if( flag & currentTlbSets ) {
			printf( "setting vpn for set flag %x\n", flag );
			SetNewVpn( value, currentTlbIndex, i );
		}
	}
}

void PpcCpu::MmuContext::WriteTlbRpn( uint64_t value )
{
	uint64_t rpn = value & 0xFFFFFFFFFFFF0000UL;
	printf( "tlbIndex[%d][%x] rpn=%016lx\n", currentTlbIndex, currentTlbSets, rpn );
	for( int i = 0; i < 4; i++ ) {
		const int flag = 0x8 >> i;
		if( flag & currentTlbSets ) {
			printf( "setting rpn for set flag %x\n", flag );
			tlbEntries[ currentTlbIndex ][ i ].rpn = value;
		}
	}
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
	printf( "   cr         %08x |   xer %16lx |   dar %16lx\n", context.cr, context.xer, context.dar );
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

void PpcCpu::OnInterpPageFault( PageFaultReason /*reason*/, uint64_t addr, InterruptRegs *regs )
{
	regs->rdi = error_context.mc_rdi;
	regs->rsi = error_context.mc_rsi;
	regs->rdx = error_context.mc_rdx;
	regs->rcx = error_context.mc_rcx;
	regs->r8  = error_context.mc_r8;
	regs->r9  = error_context.mc_r9;
	regs->rax = error_context.mc_rax;
	regs->rbx = error_context.mc_rbx;
	regs->rbp = error_context.mc_rbp;
	regs->r10 = error_context.mc_r10;
	regs->r11 = error_context.mc_r11;
	regs->r12 = error_context.mc_r12;
	regs->r13 = error_context.mc_r13;
	regs->r14 = error_context.mc_r14;
	regs->r15 = error_context.mc_r15;
	regs->rip = error_context.mc_rip;
	regs->rsp = error_context.mc_rsp;

	faultingAddr = addr;
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
		case jitpp::PowerPCHelpers::SPR_XER: {
			context.xer = value;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_SRR0: {
			context.srr0 = value;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_SRR1: {
			context.srr1 = value;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_SPRG1: {
			context.sprg1 = value;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_HRMOR: {
			printf( "PpcCpu:  HRMOR set to %016lx\n", value );
			context.hrmor = value & 0x000003FFFFF00000;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_LPCR: {
			printf( "PpcCpu:  LPCR set to %016lx\n", value );
			context.lpcr = value;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_LPIDR: {
			printf( "PpcCpu:  LPIDR set to %016lx\n", value );
			context.lpidr = value;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_PPE_TLB_INDEX: {
			printf( "PpcCpu:  PPE_TLB_Index set to %016lx\n", value );
			mmuContext.WriteTlbIndex( value );
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_PPE_TLB_VPN: {
			printf( "PpcCpu:  PPE_TLB_Vpn set to %016lx\n", value );
			mmuContext.WriteTlbVpn( value );
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_PPE_TLB_RPN: {
			printf( "PpcCpu:  PPE_TLB_Rpn set to %016lx\n", value );
			mmuContext.WriteTlbRpn( value );
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_HID6: {
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
		case jitpp::PowerPCHelpers::SPR_XER: {
			value = context.xer;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_DAR: {
			value = context.dar;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_SRR0: {
			value = context.srr0;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_LPCR: {
			value = context.lpcr;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_PPE_TLB_INDEX_HINT: {
			value = mmuContext.GetTlbHint();
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_HID6: {
			value = context.hid6;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_PIR: {
			value = context.coreNum;
			return true;
		}

		case jitpp::PowerPCHelpers::SPR_MSR: {
			value = context.msr;
			return true;
		}

		default: {
			printf( "Unknown Ppc System Reg %d read from\n", sysReg );
			return false;
		}
	}
}

bool PpcCpu::InterpretProcessorSpecific( jitpp::InterInstr &instr )
{
	switch( instr.op ) {
		case jitpp::PPC_SLBIA: {
			printf( "SLBIA\n" );
			return true;
		}

		case jitpp::PPC_SLBMTE: {
			const int rs = instr.args[0];
			const int rb = instr.args[1];

			printf( "SLBMTE RS=%016lx RB=%016lx\n", ReadGPR64(rs), ReadGPR64(rb) );
			return true;
		}

		case jitpp::PPC_TLBIEL: {
			const int rb = instr.args[0];
			const bool l = instr.args[1];

			printf( "TLBIEL RB=%016lx L=%d\n", ReadGPR64(rb), l );
			return true;
		}

		case jitpp::PPC_STWCX: {
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

		case jitpp::PPC_CMPD: {
			const int ra = instr.args[0];
			const int rb = instr.args[1];
			const int cr = instr.args[2];

			const int32_t value_a = (int32_t)ReadGPR32( ra );
			const int32_t value_b = (int32_t)ReadGPR32( rb );

			context.ClearCr( cr );

			if( value_a < value_b ) {
				context.SetCrLt( cr );
			}
			else if( value_a > value_b ) {
				context.SetCrGt( cr );
			}
			else {
				context.SetCrEq( cr );
			}

			//TODO:  Here we need to set cr so

			return true;
		}

		case jitpp::PPC_CMPDI: {
			const int ra = instr.args[0];
			const int cr = instr.args[1];
			const int32_t imm = instr.args[2];

			const int64_t value = (int64_t)ReadGPR64( ra );

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

		case jitpp::PPC_CMPW: {
			const int ra = instr.args[0];
			const int rb = instr.args[1];
			const int cr = instr.args[2];

			const int32_t value_a = (int32_t)ReadGPR32( ra );
			const int32_t value_b = (int32_t)ReadGPR32( rb );

			context.ClearCr( cr );

			if( value_a < value_b ) {
				context.SetCrLt( cr );
			}
			else if( value_a > value_b ) {
				context.SetCrGt( cr );
			}
			else {
				context.SetCrEq( cr );
			}

			//TODO:  Here we need to set cr so

			return true;
		}

		case jitpp::PPC_CMPWI: {
			const int ra = instr.args[0];
			const int cr = instr.args[1];
			const int32_t imm = instr.args[2];

			const int32_t value = (int32_t)ReadGPR32( ra );

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

		case jitpp::PPC_CMPLD: {
			const int ra = instr.args[0];
			const int rb = instr.args[1];
			const int cr = instr.args[2];

			const uint64_t value_a = ReadGPR64( ra );
			const uint64_t value_b = ReadGPR64( rb );

			context.ClearCr( cr );

			if( value_a < value_b ) {
				context.SetCrLt( cr );
			}
			else if( value_a > value_b ) {
				context.SetCrGt( cr );
			}
			else {
				context.SetCrEq( cr );
			}

			//TODO:  Here we need to set cr so

			return true;
		}

		case jitpp::PPC_CMPLDI: {
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

		case jitpp::PPC_CMPLW: {
			const int ra = instr.args[0];
			const int rb = instr.args[1];
			const int cr = instr.args[2];

			const uint32_t value_a = ReadGPR32( ra );
			const uint32_t value_b = ReadGPR32( rb );

			context.ClearCr( cr );

			if( value_a < value_b ) {
				context.SetCrLt( cr );
			}
			else if( value_a > value_b ) {
				context.SetCrGt( cr );
			}
			else {
				context.SetCrEq( cr );
			}

			//TODO:  Here we need to set cr so

			return true;
		}

		case jitpp::PPC_CMPLWI: {
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

		case jitpp::PPC_RFID: {
			SetMsr( context.srr1 );
			SetPC( context.srr0 );

			return true;
		}

		case jitpp::PPC_SRAWI: {
			const int rs = instr.args[0];
			const int ra = instr.args[1];
			const int sh = instr.args[2];
			const bool rc = instr.args[3];

			if( rc ) {
 				printf( "Actually implement srawi.\n" );
				return false;
			}

			int32_t value = (int32_t)ReadGPR32(rs);

			int64_t shifted = (value >> sh);

			if( sh == 0 ) {
				context.ClearXerCa();
			}
			else {
				uint32_t mask = 0xFFFFFFFFUL >> (32 - sh);
				if( (mask & value) != 0 ) {
					context.SetXerCa();
				}
				else {
					context.ClearXerCa();
				}
			}

			SetGPR64( ra, shifted );

			return true;
		}

		case jitpp::PPC_SUBFIC: {
			const int rt = instr.args[0];
			const int ra = instr.args[1];
			const int64_t imm = instr.args[2];

			const int64_t source = ~ReadGPR64(ra);

			SetGPR64( rt, (source) + imm + 1 );

			if( (source + imm) < source ) {
				context.SetXerCa();
			}
			else if( (source + imm + 1) < 1 ) {
				context.SetXerCa();
			}
			else {
				context.ClearXerCa();
			}

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

	jitpp::InterInstr intermediates[ 10 ];

	int status;

	mm.RegisterLowerHandler( &interpPageFaultHandler );

	if( (status = getmcontext(&error_context)) != 0) {
		if( readingInstructions ) {
			printf( "PpcCpu:  ISI @ %08lx\n", context.pc );
			context.srr0 = context.pc;
			context.srr1 = context.msr;
			SetMsr( context.msr & ~0x30 );
			SetPC( 0x400 );
		}
		else {
			context.pc -= sizeof(uint32_t);
			printf( "PpcCpu:  DSI @ %08lx(accessing %08lx)\n", context.pc, faultingAddr );
			context.srr0 = context.pc;
			context.srr1 = context.msr;
			if( (context.msr & 0x8000000000000000UL) != 0 ) {
				context.dar = faultingAddr;
			}
			else {
				context.dar = faultingAddr & 0x00000000FFFFFFFFUL;
			}

			SetMsr( context.msr & ~0x30 );
			SetPC( 0x300 );
		}
	}

	while( running ) {
		int numIntermediates = 0;

		{//Fetch phase
			readingInstructions = true;

			const uint32_t instruction = *PC;

			//DumpContext();
			//printf( "%08lx : %04x\n", context.pc, instruction );

			numIntermediates = BuildIntermediate( intermediates, instruction, context.pc );

			context.pc += sizeof(uint32_t);
		}

		{//Execute phase
			readingInstructions = false;

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
}

