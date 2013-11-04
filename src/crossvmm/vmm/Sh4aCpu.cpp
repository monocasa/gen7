#include "InterruptManager.h"
#include "MemoryManager.h"
#include "Sh4aCpu.h"

#include "util/BitOps.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

extern "C" void hyper_quit();

class NativeInstructionEmulator
{
private:
	InterruptRegs &regs;
	MemoryEmulator &mem;

public:
	void EmulateSingle();

	NativeInstructionEmulator( InterruptRegs &regs, MemoryEmulator &mem )
	  : regs( regs )
	  , mem( mem )
	{ }
};

void NativeInstructionEmulator::EmulateSingle()
{
	uint8_t * const op = (uint8_t* const)regs.rip;

	if( 0x0F == op[0] && 0xb7 == op[1] && 0x00 == op[2] ) { //movzwl (%rax),%eax
		regs.rax = mem.Read16( regs.rax );
		regs.rip += 3;
	}
	else if( 0x66 == op[0] && 0x89 == op[1] && 0x10 == op[2] ) { //mov %dx,(%rax)
		mem.Write16( regs.rax, (uint16_t)regs.rdx );
		regs.rip += 3;
	}
	else if( 0x88 == op[0] && 0x10 == op[1] ) { // mov %dl, (%rax)
		mem.Write8( regs.rax, (uint8_t)regs.rdx );
		regs.rip += 2;
	}
	else if( 0x89 == op[0] && 0x10 == op[1] ) {	 // mov %edx, (%rax)
		mem.Write32( regs.rax, (uint32_t)regs.rdx );
		regs.rip += 2;
	}
	else if( 0x8b == op[0] && 0x0a == op[1] ) {	 // mov (%rdx), %ecx
		regs.rcx = mem.Read32( regs.rdx );
		regs.rip += 2;
	}
	else {
		printf( "Unknown native op @ %p\n", op );
		hyper_quit();
	}
}

void Sh4aCpu::MmuContext::P4Access( uint64_t /*addr*/, InterruptRegs *regs )
{
	NativeInstructionEmulator nativeEmu( *regs, *this );
	nativeEmu.EmulateSingle();
}

void Sh4aCpu::MmuContext::Init()
{
	pml3 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

	for( int i = 0; i < NUM_PML2S; i++ ) {
		pml2s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	}

	for( int i = 0; i < NUM_PML1S; i++ ) {
		pml1s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	}

	//ccnPml1 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	//ccnPage = reinterpret_cast<uint32_t*>( mm.AllocatePage() );

	//bscPml1 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	//bscPage = reinterpret_cast<uint32_t*>( mm.AllocatePage() );

	//sdmrPage = reinterpret_cast<uint32_t*>( mm.AllocatePage() );

	// Set EXPEVT to say that we're coming out of soft reset
	//ccnPage[ 0x24/sizeof(uint32_t) ] = 0x00000020;
}

void Sh4aCpu::MmuContext::MapFull()
{
	for( int i = 0; i < 4; i++ ) {
		pml3[ i ] = (((uint64_t)pml2s[ i ]) - 0xFFFFFFFF80000000UL) | 1;
	}

	for( int i = 0; i < 256; i++ ) {
		pml2s[ 0 ][ i       ] = 0;
		pml2s[ 0 ][ i + 256 ] = 0;

		pml2s[ 1 ][ i       ] = 0;
		pml2s[ 1 ][ i + 256 ] = 0;

		pml2s[ 2 ][ i       ] = (0x100000000UL + ((2 * 1024 * 1024UL) * i)) | 0x83;
		pml2s[ 2 ][ i + 256 ] = (0x100000000UL + ((2 * 1024 * 1024UL) * i)) | 0x83;

		pml2s[ 3 ][ i       ] = 0;
		pml2s[ 3 ][ i + 256 ] = 0;
	}

//	pml2s[ 3 ][ 504 ] = (((uint64_t)ccnPml1) - 0xFFFFFFFF80000000UL) | 1;
//	ccnPml1[ 0 ] = (((uint64_t)ccnPage) - 0xFFFFFFFF80000000UL) | 0x83;

//	pml2s[ 3 ][ 508 ] = (((uint64_t)bscPml1) - 0xFFFFFFFF80000000UL) | 1;
//	bscPml1[   0 ] = (((uint64_t)bscPage) - 0xFFFFFFFF80000000UL) | 0x83;
//	bscPml1[ 256 ] = (((uint64_t)sdmrPage) - 0xFFFFFFFF80000000UL) | 0x83;
//	bscPml1[ 320 ] = (((uint64_t)sdmrPage) - 0xFFFFFFFF80000000UL) | 0x83;

	mm.SetLowerPml3( pml3, 0x0000000000000000UL );

	mm.RegisterLowerHandler( this );
}

void Sh4aCpu::MmuContext::OnPageFault( PageFaultReason /*reason*/, uint64_t addr, InterruptRegs *regs )
{
	if( addr > 0xFFFFFFFF ) {
		printf( "Woah massive error.  Addr=%016lx rip=%016lx\n", addr, regs->rip );
		hyper_quit();
	}
	else if( addr >= 0xE0000000 ) {
		P4Access( addr, regs );
	}
	else {
		printf( "Unimplemented access Addr=%016lx\n", addr );
		cpu.DumpState();
		hyper_quit();
	}
}

void Sh4aCpu::MmuContext::Write8( uint64_t addr, uint8_t data )
{
	switch( addr ) {
		case 0xFF940190: {
			printf( "Area 3 set to width=64 cas=3\n" );
			return;
		}

		default: {
			printf( "Implement Sh4aCpu::MmuContext::Write8( addr=%08lx, data=%02x )\n", addr, data );
			hyper_quit();
		}
	}
}

void Sh4aCpu::MmuContext::Write16( uint64_t addr, uint16_t data )
{
	switch( addr ) {
		case 0xFF800004: {
			printf( "Write to BCR2:  %04x\n", data );
			return;
		}

		case 0xFF80001C: {
			printf( "Write to RTCSR: %04x\n", data );
			return;
		}

		case 0xFF800024: {
			printf( "Write to RTCOR: %04x\n", data );
			return;
		}

		case 0xFF800028: {
			printf( "Write to RFCR:  %04x\n", data );
			return;
		}

		default: {
			printf( "Implement Sh4aCpu::MmuContext::Write16( addr=%08lx, data=%04x )\n", addr, data );
			hyper_quit();
		}
	}
}

uint16_t Sh4aCpu::MmuContext::Read16( uint64_t addr )
{
	switch( addr ) {
		case 0xFF800028: {
			printf( "Faking out RFCR\n" );
			return 0xFFFF;
		}

		default: {
			printf( "Implement Sh4aCpu::MmuContext::Read16( addr=%08lx )\n", addr );
			hyper_quit();
			return 0;
		}
	}
}

void Sh4aCpu::MmuContext::Write32( uint64_t addr, uint32_t data )
{
	if( (addr & 0xF0000000) == 0xE0000000 ) {
		context.sq[ (addr >> 2) & 0xF ] = data;
		return;
	}

	switch( addr ) {
		case 0xFF00001C: {
			printf( "Write to CCR:  %08x\n", data );
			return;
		}

		case 0xFF000010: {
			printf( "Write to MMUCR:  %08x\n", data );
			return;
		}

		case 0xFF800000: {
			printf( "Write to BCR1:  %08x\n", data );
			return;
		}

		case 0xFF800008: {
			printf( "Write to WCR1:  %08x\n", data );
			return;
		}

		case 0xFF80000C: {
			printf( "Write to WCR2:  %08x\n", data );
			return;
		}

		case 0xFF800014: {
			printf( "Write to MCR:  %08x\n", data );
			return;
		}

		case 0xFF000038: {
			context.qacr0 = data & 0x1C;
			return;
		}

		case 0xFF00003C: {
			context.qacr1 = data & 0x1C;
			return;
		}

		default: {
			printf( "Implement Sh4aCpu::MmuContext::Write32( addr=%08lx, data=%08x )\n", addr, data );
			hyper_quit();
		}
	}
}

uint32_t Sh4aCpu::MmuContext::Read32( uint64_t addr )
{
	if( (addr & 0xF0000000) == 0xE0000000 ) {
		return context.sq[ (addr >> 2) & 0xF ];
	}

	switch( addr ) {
		case 0xFF000024: {
			return context.expevt;
		}

		default: {
			printf( "Implement Sh4aCpu::MmuContext::Read32( addr=%08lx )\n", addr );
			hyper_quit();
			return 0;
		}
	}
}

void Sh4aCpu::DumpState()
{
	printf( " r0 %8x |  r1 %8x |  r2 %8x |  r3 %8x\n", context.gpr[ 0], context.gpr[ 1], context.gpr[ 2], context.gpr[ 3] );
	printf( " r4 %8x |  r5 %8x |  r6 %8x |  r7 %8x\n", context.gpr[ 4], context.gpr[ 5], context.gpr[ 6], context.gpr[ 7] );
	printf( " r8 %8x |  r9 %8x | r10 %8x | r11 %8x\n", context.gpr[ 8], context.gpr[ 9], context.gpr[10], context.gpr[11] );
	printf( "r12 %8x | r13 %8x | r14 %8x | r15 %8x\n", context.gpr[12], context.gpr[13], context.gpr[14], context.gpr[15] );
	printf( " pc %08x |  sr %08x |  pr %08x\n", context.pc, context.sr, context.pr );
	printf( "fpscr %06x\n", context.fpscr );

	if( (context.fpscr & FPSCR_SZ_BIT) == 0 ) {
		for( int i = 0; i < 16; i++ ) {
			printf( "%sf%d %08x%s",
			        (i < 10) ? " " : "",
			        i,
			        *((uint32_t*)&context.fpr[i]),
			        ((i % 4) == 3) ? "\n" : " | " );
		}

		for( int i = 0; i < 16; i++ ) {
			printf( "%sx%d %08x%s",
			        (i < 10) ? " " : "",
			        i,
			        *((uint32_t*)&context.xpr[i]),
			        ((i % 4) == 3) ? "\n" : " | " );
		}
	}
}

void Sh4aCpu::SetSR( uint32_t newValue )
{
	uint32_t reservedBits = newValue & ~SR_ALL;

	if( reservedBits != 0 ) {
		printf( "Unknown bits set in SR %08x\n", reservedBits );
		hyper_quit();
	}

	if( (newValue & SR_MD_BIT) == 0 ) {
		printf( "Switch to unprivileged mode not implemented\n" );
		hyper_quit();
	}

//	if( (newValue & SR_BL_BIT) == 0 ) {
//		printf( "Interrupt block disabled not implemented\n" );
//		hyper_quit();
//	}


	if( (newValue & SR_RB_BIT) != (context.sr & SR_RB_BIT) ) {
		uint32_t temp[8];
		memcpy( temp,                 &context.gpr[0],      sizeof(uint32_t) * 8 );
		memcpy( &context.gpr[0],      &context.gpr_bank[0], sizeof(uint32_t) * 8 );
		memcpy( &context.gpr_bank[0], temp,                 sizeof(uint32_t) * 8 );
	}

	context.sr = newValue;
}

void Sh4aCpu::Init()
{
	mmu.Init();
	mmu.MapFull();

	SetSR( context.sr );
}

void Sh4aCpu::Execute()
{
	bool running = true;

	enum BranchDelayState {
		NO_DELAY,
		ENTERING_DELAY,
		EXITING_DELAY,
	};

	BranchDelayState delayState = NO_DELAY;
	uint32_t delayTarget = 0;

	while( running ) {
		uint16_t opcode = *reinterpret_cast<uint16_t*>( context.pc );

		//printf( "%08x : %04x\n", context.pc, opcode );
		//
		//if( 0x8c010810 <= context.pc && 0x8c010974 >= context.pc ) {
		//	printf( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
		//	DumpState();
		//}

		switch( opcode & 0xF000 ) {
			case 0x0000: {
				switch( opcode & 0x000F ) {
					case 0x2: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { // stc sr, Rn
								int rn = (opcode >> 8) & 0xF;
								context.gpr[rn] = context.sr;
								break;
							}

							default: {
								printf( "Unknown 0x0002 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x3: {
						switch( opcode & 0x00F0 ) {
							case 0x0020: { //braf rn
								int rn = (opcode >> 8) & 0xF;

								delayTarget = context.gpr[rn] + context.pc + 4;

								delayState = ENTERING_DELAY;

								break;
							}

							case 0x0080: { //pref @rn
								int rn = (opcode >> 8) & 0xF;
								if( context.gpr[rn] > 0xF0000000 ) {
									printf( "Start looking at store queues\n" );
									running = false;
								}
								break;
							}

							default: {
								printf( "unknown 0x0003 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x6: { //mov.l Rm, @(R0,Rn)
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint32_t *ptr = (uint32_t*)((uint64_t)(context.gpr[0] + context.gpr[rn]));

						*ptr = context.gpr[rm];

						break;
					}

					case 0x9: {
						if( opcode == 0x0009 ) { // nop
							break;
						}

						else {
							printf( "Unknown 0x0009 opcode %04x\n", opcode );
							running = false;
							break;
						}
						break;
					}

					case 0xB: {
						if( 0x000B == opcode ) { // rts
							delayTarget = context.pr;
							delayState = ENTERING_DELAY;

							printf( "Returning to %x\n", context.pr );
							break;
						}
						else {
							printf( "Unknown opcode %04x\n", opcode );
							running = false;
							break;
						}
						break;
					}

					case 0xA: {
						switch( opcode & 0x00F0 ) {
							case 0x0010: { //sts macl, rn
								int rn = (opcode >> 8) & 0xF;
								context.gpr[ rn ] = context.macl;
								break;
							}

							case 0x0020: { //sts pr, rn
								int rn = (opcode >> 8) & 0xF;
								context.gpr[ rn ] = context.pr;
								break;
							}

							default: {
								printf( "Unknown 0x000A opcode %04x\n", opcode ); 
								running = false;
								break;
							}
						}
						break;
					}

					case 0xE: { //mov.l @(R0,Rm), Rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint32_t *ptr = (uint32_t*)((uint64_t)(context.gpr[0] + context.gpr[rm]));

						context.gpr[rn] = *ptr;

						break;
					}

					default: {
						printf( "Unknown 0x0 opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}

			case 0x1000: { //mov.l Rm,@(disp, Rn)
				int rn = (opcode >> 8) & 0xF;
				int rm = (opcode >> 4) & 0xF;
				int disp = opcode & 0xF;

				uint64_t ptr = context.gpr[rn] + (disp * 4);

				*((uint32_t*)ptr) = context.gpr[rm];

				break;
			}
	
			case 0x2000: {
				switch( opcode & 0x000F ) {
					case 0x0000: { //mov.b rm, @rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint8_t *ptr = (uint8_t*)((uint64_t)context.gpr[rn]);
						*ptr = context.gpr[rm];

						break;
					}

					case 0x0001: { //mov.w rm, @rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint16_t *ptr = (uint16_t*)((uint64_t)context.gpr[rn]);
						*ptr = context.gpr[rm];

						break;
					}

					case 0x0002: { //mov.l rm, @rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint32_t *ptr = (uint32_t*)((uint64_t)context.gpr[rn]);
						*ptr = context.gpr[rm];

						break;
					}

					case 0x0005: { //mov.w rm, @-rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint16_t *ptr = (uint16_t*)((uint64_t)context.gpr[rn] -2);
						*ptr = context.gpr[rm];
						context.gpr[rn] -= 2;

						break;
					}

					case 0x0006: { //mov.l rm, @-rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint32_t *ptr = (uint32_t*)((uint64_t)context.gpr[rn] - 4);
						*ptr = context.gpr[rm];
						context.gpr[rn] -= 4;

						break;
					}

					case 0x0008: { //tst rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;
						if( (context.gpr[ rn ] & context.gpr[ rm ]) == 0 ) {
							context.sr |= SR_T_BIT;
						}
						else {
							context.sr &= ~SR_T_BIT;
						}
						break;
					}

					case 0x0009: { //and rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;
						context.gpr[ rn ] = context.gpr[ rn ] & context.gpr[ rm ];
						break;
					}

					case 0x000A: { //xor rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;
						context.gpr[ rn ] = context.gpr[ rn ] ^ context.gpr[ rm ];
						break;
					}

					case 0x000B: { //or rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;
						context.gpr[ rn ] = context.gpr[ rn ] | context.gpr[ rm ];
						break;
					}

					case 0x000E: { //mulu.w rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.macl = (context.gpr[rn] & 0xFFFF) * (context.gpr[rm] & 0xFFFF);

						break;
					}

					default: {
						printf( "Unknown 0x2 opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}
	
			case 0x3000: {
				switch( opcode & 0x000F ) {
					case 0x0000: { //CMP/EQ Rm,Rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						if( context.gpr[rn] == context.gpr[rm] ) {
							context.sr |= SR_T_BIT;
						}
						else {
							context.sr &= ~SR_T_BIT;
						} 
						break;
					}

					case 0x0003: { //CMP/GE Rm,Rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						if( ((int32_t)context.gpr[rn]) >= ((int32_t)context.gpr[rm]) ) {
							context.sr |= SR_T_BIT;
						}
						else {
							context.sr &= ~SR_T_BIT;
						} 
						break;
					}

					case 0x0006: { //CMP/HI Rm,Rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						if( context.gpr[rn] > context.gpr[rm] ) {
							context.sr |= SR_T_BIT;
						}
						else {
							context.sr &= ~SR_T_BIT;
						} 
						break;
					}

					case 0x0008: { //SUB Rm, Rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.gpr[rn] -= context.gpr[rm];

						break;
					}

					case 0x000C: { //ADD Rm, Rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.gpr[rn] += context.gpr[rm];

						break;
					}

					default: {
						printf( "Unknown 0x3 opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}

			case 0x4000: {
				switch( opcode & 0x000F ) {
					case 0x00: {
						switch( opcode & 0x00F0 ) {
							case 0x0010: { //dt rn
								int rn = (opcode >> 8) & 0xF;

								context.gpr[rn]--;

								if( 0 == context.gpr[rn] ) {
									context.sr |= SR_T_BIT;
								}
								else {
									context.sr &= ~SR_T_BIT;
								}

								break;
							}

							default: {
								printf( "Unknown 0x4000 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x01: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: {
								int rn = (opcode >> 8) & 0xF;

								context.sr &= ~SR_T_BIT;
								context.sr |= context.gpr[rn] & SR_T_BIT;

								context.gpr[rn] >>= 1;

								break;
							}
		
							case 0x0020: {
								int rn = (opcode >> 8) & 0xF;

								context.sr &= ~SR_T_BIT;
								context.sr |= context.gpr[rn] & SR_T_BIT;

								uint32_t temp = context.gpr[rn] >> 1;
								temp |= context.gpr[rn] & 0x80000000;
								context.gpr[rn] = temp;

								break;
							}

							default: {
								printf( "Unknown 0x4001 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x02: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { // STS.L mach @-Rn
								int rn = (opcode >> 8) & 0xF;

								uint32_t *ptr = (uint32_t*)((uint64_t)context.gpr[rn] - 4);
								*ptr = context.mach;
								context.gpr[rn] -= sizeof(uint32_t);

								break;
							}

							case 0x0010: { // STS.L macl @-Rn
								int rn = (opcode >> 8) & 0xF;

								uint32_t *ptr = (uint32_t*)((uint64_t)context.gpr[rn] - 4);
								*ptr = context.macl;
								context.gpr[rn] -= sizeof(uint32_t);

								break;
							}

							case 0x0020: { // STS.L pr @-Rn
								int rn = (opcode >> 8) & 0xF;

								uint32_t *ptr = (uint32_t*)((uint64_t)context.gpr[rn] - 4);
								*ptr = context.pr;
								context.gpr[rn] -= sizeof(uint32_t);

								break;
							}

							default: {
								printf( "Unknown 0x4002 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x5: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: {
								int rn = (opcode >> 8) & 0xF;

								context.sr &= ~SR_T_BIT;
								context.sr |= context.gpr[rn] & SR_T_BIT;

								context.gpr[rn] >>= 1;
								context.gpr[rn] |= (context.sr & SR_T_BIT) << 31;

								break;
							}

							default: {
								printf( "Unknown 0x4005 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x6: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { // lds.l @Rm+, MACH
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.mach = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0010: { // lds.l @Rm+, MACL
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.mach = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0020: { // lds.l @Rm+, PR
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.pr = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0050: { // lds.l @Rm+, FPUL
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.fpul = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0060: { // lds.l @Rm+, FPSCR
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.fpscr = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							default: {
								printf( "Unknown 0x4006 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x7: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { // lds.l @Rm+, SR
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								SetSR( *ptr );
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0010: { // lds.l @Rm+, GBR
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.gbr = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0020: { // lds.l @Rm+, VBR
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.vbr = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0030: { // lds.l @Rm+, SSR
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.ssr = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							case 0x0040: { // lds.l @Rm+, SPC
								int rn = (opcode >> 8) & 0xF;
								uint32_t* ptr = (uint32_t*)((uint64_t)context.gpr[rn]);

								context.spc = *ptr;
								context.gpr[rn] += sizeof(uint32_t);

								break;
							}

							default: {
								printf( "Unknown 0x4007 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x8: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { //shll2 reg
								int reg = (opcode >> 8) & 0xF;
								context.gpr[ reg ] <<= 2;
								break;
							}

							case 0x0010: { //shll8 reg
								int reg = (opcode >> 8) & 0xF;
								context.gpr[ reg ] <<= 8;
								break;
							}

							case 0x0020: { //shll16 reg
								int reg = (opcode >> 8) & 0xF;
								context.gpr[ reg ] <<= 16;
								break;
							}

							default: {
								printf( "Unknown 0x4008 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x9: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { //shlr2 reg
								int reg = (opcode >> 8) & 0xF;
								context.gpr[ reg ] >>= 2;
								break;
							}

							default: {
								printf( "Unknown 0x4009 opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0xA: {
						switch( opcode & 0x00F0 ) {
							case 0x0060: { // ldc rn, fpscr
								int rn = (opcode >> 8) & 0xF;

								context.fpscr = context.gpr[rn];

								break;
							}

							case 0x00F0: { // ldc rn, dbr
								int rn = (opcode >> 8) & 0xF;

								context.dbr = context.gpr[rn];

								break;
							}

							default: {
								printf( "Unknown 0x400A opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0xB: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { //jsr @rn
								int rn = (opcode >> 8) & 0xF;

								context.pr = context.pc + 4;

								delayTarget = context.gpr[rn];

								delayState = ENTERING_DELAY;

								printf( "Calling loc_%x\n", delayTarget );

								DumpState();

								break;
							}

							case 0x0020: { //jmp @rn
								int rn = (opcode >> 8) & 0xF;

								delayTarget = context.gpr[rn];

								delayState = ENTERING_DELAY;

								break;
							}

							default: {
								printf( "Unknown 0x400B opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0xC: {
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						if( context.gpr[rn] & 0x80000000 ) {
							printf( "Implement shad sign extend\n" );
							running = false;
						}
						else {
							context.gpr[rn] <<= (int)context.gpr[rm];
						}

						break;
					}

					case 0xD: {
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.gpr[rn] <<= (int)context.gpr[rm];

						break;
					}

					case 0xE: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: { // ldc rn, sr
								int rn = (opcode >> 8) & 0xF;

								SetSR( context.gpr[rn] );

								break;
							}

							default: {
								printf( "Unknown 0x400E opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					default: {
						printf( "Unknown 0x4 opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}

			case 0x5000: { //mov.l @(disp, Rm),Rn
				int rn = (opcode >> 8) & 0xF;
				int rm = (opcode >> 4) & 0xF;
				int disp = opcode & 0xF;

				uint64_t ptr = context.gpr[rm] + (disp * 4);

				context.gpr[rn] = *((uint32_t*)ptr);

				break;
			}

			case 0x6000: {
				switch( opcode & 0x000F ) {
					case 0x2: { //mov.l @rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint32_t *ptr = (uint32_t*)((uint64_t)context.gpr[rm]);

						context.gpr[rn] = *ptr;

						break;
					}

					case 0x3: { //mov rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.gpr[rn] = context.gpr[rm];

						break;
					}

					case 0x5: { //mov.w @rm+, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint16_t *ptr = (uint16_t*)((uint64_t)context.gpr[rm]);

						context.gpr[rn] = util::SignExtend<int32_t,16>( *ptr );

						context.gpr[rm] += 2;

						break;
					}

					case 0x6: { //mov.l @rm+, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						uint32_t *ptr = (uint32_t*)((uint64_t)context.gpr[rm]);

						context.gpr[rn] = *ptr;

						context.gpr[rm] += 4;

						break;
					}

					case 0x7: { //not rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.gpr[rn] = ~context.gpr[rm];

						break;
					}

					case 0x8: { //swap.b rm, rn	
						int dest = (opcode >> 8) & 0xF;
						int src = (opcode >> 4) & 0xF;

						uint32_t temp = context.gpr[ src ] & 0xFFFF0000;
						temp |= (context.gpr[ src ] << 8) & 0x000FF00;
						temp |= (context.gpr[ src ] >> 8) & 0x00000FF;

						context.gpr[dest] = temp;

						break;
					}

					case 0x9: { //swap.w rm, rn
						int dest = (opcode >> 8) & 0xF;
						int src = (opcode >> 4) & 0xF;

						uint32_t temp = context.gpr[ src ] >> 16;
						temp |= context.gpr[ src ] << 16;

						context.gpr[dest] = temp;

						break;
					}

					case 0xA: { //negc rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						int temp = 0 - context.gpr[rm];
						context.gpr[rn] = temp - (context.sr & SR_T_BIT);
						if( 0 < temp ) {
							context.sr |= SR_T_BIT;
						}
						else {
							context.sr &= ~SR_T_BIT;
						}

						if( temp < ((int)context.gpr[rn]) ) {
							context.sr |= SR_T_BIT;
						}

						break;
					}

					case 0xB: { //neg rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.gpr[rn] = 0 - context.gpr[rm];

						break;
					}

					case 0xC: { //extu.b rm,rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						context.gpr[rn] = context.gpr[rm] & 0x000000FF;

						break;
					}

					default: {
						printf( "Unknown 0x6 opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}

			case 0x7000: {
				int rn = (opcode >> 8) & 0xF;
				int32_t imm = opcode & 0xFF;

				imm = util::SignExtend<int32_t,8>( imm );

				context.gpr[rn] += imm;

				break;
			}

			case 0x8000: {
				switch( opcode & 0x0F00 ) {
					case 0x0100: { //mov.w (r0,Rm),r0
						int rn = (opcode >> 4) & 0xF;
						int disp = (opcode & 0xF) * sizeof(uint16_t);

						uint64_t ptr = context.gpr[rn] + disp;

						*((uint16_t*)ptr) = context.gpr[0];

						break;
					}

					case 0x0500: { //mov.w @(disp,rm), r0
						int rm = (opcode >> 4) & 0xf;
						int disp = (opcode & 0xF) * sizeof(uint16_t);

						uint64_t ptr = context.gpr[rm] + disp;

						context.gpr[0] = util::SignExtend<int32_t,16>( *((int16_t*)ptr) );

						break;
					}

					case 0x0900: { //bt loc_disp
						int disp = opcode & 0xFF;
						disp *= 2;
						disp = util::SignExtend<int32_t,9>( disp );
						uint32_t target = context.pc + disp + 4;

						if( (context.sr & SR_T_BIT) == SR_T_BIT ) {
							context.pc = target - 2;
						}

						break;
					}

					case 0x0B00: { //bf loc_disp
						int disp = opcode & 0xFF;
						disp *= 2;
						disp = util::SignExtend<int32_t,9>( disp );
						uint32_t target = context.pc + disp + 4;

						if( (context.sr & SR_T_BIT) == 0 ) {
							context.pc = target - 2;
						}

						break;
					}

					case 0x0F00: { //bf/s loc_disp
						if( (context.sr & SR_T_BIT) == 0 ) {
							int disp = opcode & 0xFF;
							disp *= 2;
							disp = util::SignExtend<int32_t,9>( disp );

							delayTarget = context.pc + disp + 4;

							delayState = ENTERING_DELAY;
						}

						break;
					}

					default: {
						printf( "Unknown 0x8 opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}

			case 0x9000: { //mov.w @(disp,PC),Rn
				int rn = (opcode >> 8) & 0xF;
				int disp = opcode & 0xFF;
				disp *= 2;

				uint64_t ptr = context.pc + disp + 4;
				uint16_t value = *reinterpret_cast<int16_t*>( ptr );

				context.gpr[rn] = util::SignExtend<int32_t,16>( value );

				break;
			}

			case 0xA000: {
				int disp = util::SignExtend<int,12>( opcode & 0xFF ) * 2;
				uint32_t target = context.pc + 4 + disp;

				delayTarget = target;
				delayState = ENTERING_DELAY;

				break;
			}

			case 0xC000: { 
				switch( opcode & 0x0F00 ) {
					case 0x0700: { //mova @(disp,pc),r0
						int disp = opcode & 0xFF;
						disp *= 4;

						context.gpr[0] = context.pc + 4 + disp;

						break;
					}

					case 0x0800: { //tst #imm, r0
						uint32_t imm = opcode & 0xFF;

						if( (context.gpr[0] & imm) == 0 ) {
							context.sr |= SR_T_BIT;
						}
						else {
							context.sr &= ~SR_T_BIT;
						}

						break;
					}

					case 0x0A00: { //or #imm, R0
						int imm = opcode & 0x00FF;

						context.gpr[0] ^= imm;

						break;
					}

					case 0x0B00: { //or #imm, R0
						int imm = opcode & 0x00FF;

						context.gpr[0] |= imm;

						break;
					}

					default: {
						printf( "Unknown 0xC opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}

			case 0xD000: { //mov.l @(disp,pc),rn
				int rn = (opcode >> 8) &0xF;

				int disp = (opcode & 0x00FF) * 4;

				uint64_t ptr = context.pc + disp + 4;
				ptr &= 0xFFFFFFFC;

				context.gpr[rn] = *((uint32_t*)ptr);

				break;
			}

			case 0xE000: { //mov #imm, reg
				int32_t imm = util::SignExtend<int32_t,8>( opcode & 0xFF );
				int reg = (opcode >> 8) & 0xF;
				context.gpr[ reg ] = imm;
				break;
			}

			case 0xF000: {
				switch( opcode & 0x000F ) {
					case 0x0009: {
						int frn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						if( (context.fpscr & FPSCR_SZ_BIT) == 0 ) {
							printf( "fmov.s @r%d+, fr%d\n", rm, frn );
							running = false;
							break;
						}
						else {
							frn >>= 1;
							if( ((opcode >> 8) & 1) == 0 ) {
								uint64_t * memPtr = (uint64_t*)( (uint64_t)context.gpr[rm] );
								uint64_t * fprPtr = (uint64_t*)( &context.fpr[frn * 2] );

								*fprPtr = *memPtr;

								context.gpr[rm] += 8;

								break;
							}
							else {
								printf( "fmov @r%d+, xdr%d\n", rm, frn );
								running = false;
								break;
							}
						}
					}

					case 0x000D: {
						switch( opcode & 0x00F0 ) {
							case 0x00F0: {
								switch( opcode & 0x0F00 ) {
									case 0x0B00: { // frchg
										float temp[16];
										memcpy( temp,        context.fpr, sizeof(float) * 16 );
										memcpy( context.fpr, context.xpr, sizeof(float) * 16 );
										memcpy( context.xpr, temp       , sizeof(float) * 16 );
										break;
									}

									default: {
										printf( "Unknown 0xF0FD opcode %04x\n", opcode );
										running = false;
										break;
									}
								}
								break;
							}

							default: {
								printf( "Unknown 0xF00D opcode %04x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					case 0x000A: { //fmov.s FRm, @Rn
						int rn = (opcode >> 8) & 0xF;
						int frm = (opcode >> 4) & 0xF;

						float *ptr = (float*)((uint64_t)context.gpr[rn]);

						*ptr = context.fpr[frm];

						break;
					}

					case 0x000B: { //fmov.s FRm, @-Rn
						int rn = (opcode >> 8) & 0xF;
						int frm = (opcode >> 4) & 0xF;

						float *ptr = (float*)((uint64_t)context.gpr[rn] - sizeof(float));

						*ptr = context.fpr[frm];
						context.gpr[rn] -= sizeof(float);

						break;
					}

					default: {
						printf( "Unknown 0xF opcode %04x\n", opcode );
						running = false;
						break;
					}
				}
				break;
			}

			default: {
				printf( "%08x : %04x : <UNKNOWN_OPCODE>\n", context.pc, opcode );
				running = false;
				break;
			}
		}
		//DumpState();
		if( running ) {
			switch( delayState ) {
				case NO_DELAY: {
					context.pc += 2;
					break;
				}

				case ENTERING_DELAY: {
					context.pc += 2;
					delayState = EXITING_DELAY;
					break;
				}

				case EXITING_DELAY: {
					context.pc = delayTarget;
					delayState = NO_DELAY;
					break;
				}
			}
		}
	}

	DumpState();
}

