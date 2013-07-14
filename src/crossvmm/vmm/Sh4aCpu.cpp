#include "InterruptManager.h"
#include "MemoryManager.h"
#include "Sh4aCpu.h"

#include "shared/Util.h"

#include <cstdint>
#include <cstdio>

extern void hyper_quit();

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

void Sh4aCpu::MmuContext::P4Access( uint64_t addr, InterruptRegs *regs )
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

void Sh4aCpu::MmuContext::OnPageFault( PageFaultReason reason, uint64_t addr, InterruptRegs *regs )
{
	if( addr > 0xFFFFFFFF ) {
		printf( "Woah massive error.  Addr=%016lx rip=%016lx\n", addr, regs->rip );
		hyper_quit();
	}
	else if( addr >= 0xF0000000 ) {
		P4Access( addr, regs );
	}
	else {
		printf( "Unimplemented access Addr=%016lx", addr );
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

		default: {
			printf( "Implement Sh4aCpu::MmuContext::Write32( addr=%08lx, data=%08x )\n", addr, data );
			hyper_quit();
		}
	}
}

uint32_t Sh4aCpu::MmuContext::Read32( uint64_t addr )
{
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
	printf( " pc %08x |  sr %08x\n", context.pc, context.sr );
}

void Sh4aCpu::Init()
{
	mmu.Init();
	mmu.MapFull();
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

//		printf( "%08x : %04x\n", context.pc, opcode );

		switch( opcode & 0xF000 ) {
			case 0x0000: {
				switch( opcode & 0x000F ) {
					case 0x3: {
						switch( opcode & 0x00F0 ) {
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

					case 0xA: {
						switch( opcode & 0x00F0 ) {
							case 0x0010: { //sts macl, rn
								int rn = (opcode >> 8) & 0xF;
								context.gpr[ rn ] = context.macl;
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

					case 0x0008: { //tst rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;
						if( context.gpr[ rm ] & context.gpr[ rm ] ) {
							context.sr |= T_BIT;
						}
						else {
							context.sr &= ~T_BIT;
						}
						break;
					}

					case 0x000A: { //xor rm, rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;
						context.gpr[ rn ] = context.gpr[ rn ] ^ context.gpr[ rm ];
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
							context.sr |= T_BIT;
						}
						else {
							context.sr &= ~T_BIT;
						} 
						break;
					}

					case 0x0006: { //CMP/HI Rm,Rn
						int rn = (opcode >> 8) & 0xF;
						int rm = (opcode >> 4) & 0xF;

						if( context.gpr[rn] > context.gpr[rm] ) {
							context.sr |= T_BIT;
						}
						else {
							context.sr &= ~T_BIT;
						} 
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
									context.sr |= T_BIT;
								}
								else {
									context.sr &= ~T_BIT;
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

								context.sr &= ~T_BIT;
								context.sr |= context.gpr[rn] & T_BIT;

								context.gpr[rn] >>= 1;

								break;
							}
		
							case 0x0020: {
								int rn = (opcode >> 8) & 0xF;

								context.sr &= ~T_BIT;
								context.sr |= context.gpr[rn] & T_BIT;

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

					case 0x5: {
						switch( opcode & 0x00F0 ) {
							case 0x0000: {
								int rn = (opcode >> 8) & 0xF;

								context.sr &= ~T_BIT;
								context.sr |= context.gpr[rn] & T_BIT;

								context.gpr[rn] >>= 1;
								context.gpr[rn] |= (context.sr & T_BIT) << 31;

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

					case 0xB: {
						switch( opcode & 0x00F0 ) { //jmp @rn
							case 0x0020: {
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

						context.gpr[rn] = Util::SignExtend<int32_t,16>( *ptr );

						context.gpr[rm] += 2;

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
				int imm = opcode & 0xFF;

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

						context.gpr[0] = Util::SignExtend<int32_t,16>( *((int16_t*)ptr) );

						break;
					}

					case 0x0B00: { //bf loc_disp
						int disp = opcode & 0xFF;
						disp *= 2;
						disp = Util::SignExtend<int32_t,9>( disp );
						uint32_t target = context.pc + disp + 4;

						if( (context.sr & T_BIT) == 0 ) {
							context.pc = target - 2;
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

				context.gpr[rn] = Util::SignExtend<int32_t,16>( value );

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

						if( context.gpr[0] & imm ) {
							context.sr |= T_BIT;
						}
						else {
							context.sr &= ~T_BIT;
						}

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

			case 0xE000: { //mov #imm, reg
				int32_t imm = Util::SignExtend<int32_t,8>( opcode & 0xFF );
				int reg = (opcode >> 8) & 0xF;
				context.gpr[ reg ] = imm;
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

