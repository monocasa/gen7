#include "MemoryManager.h"
#include "Sh4aCpu.h"

#include "shared/Util.h"

#include <cstdint>
#include <cstdio>

void Sh4aCpu::MmuContext::Init()
{
	pml3 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

	for( int i = 0; i < NUM_PML2S; i++ ) {
		pml2s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	}

	for( int i = 0; i < NUM_PML1S; i++ ) {
		pml1s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	}
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

	mm.SetLowerPml3( pml3, 0x0000000000000000UL );
}

void Sh4aCpu::DumpState()
{
	printf( " r0 %8x |  r1 %8x |  r2 %8x |  r3 %8x\n", context.gpr[ 0], context.gpr[ 1], context.gpr[ 2], context.gpr[ 3] );
	printf( " r4 %8x |  r5 %8x |  r6 %8x |  r7 %8x\n", context.gpr[ 4], context.gpr[ 5], context.gpr[ 6], context.gpr[ 7] );
	printf( " r8 %8x |  r9 %8x | r10 %8x | r11 %8x\n", context.gpr[ 8], context.gpr[ 9], context.gpr[10], context.gpr[11] );
	printf( "r12 %8x | r13 %8x | r14 %8x | r15 %8x\n", context.gpr[12], context.gpr[13], context.gpr[14], context.gpr[15] );
	printf( " pc %8x\n", context.pc );
}

void Sh4aCpu::Init()
{
	mmu.Init();
	mmu.MapFull();
}

void Sh4aCpu::Execute()
{
	bool running = true;

	while( running ) {
		uint16_t opcode = *reinterpret_cast<uint16_t*>( context.pc );

		switch( opcode & 0xF000 ) {
			case 0x4000: {
				switch( opcode & 0x000F ) {
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
								printf( "Unknown 0x4008 opcode %x\n", opcode );
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
								printf( "Unknown 0x4009 opcode %x\n", opcode );
								running = false;
								break;
							}
						}
						break;
					}

					default: {
						printf( "Unknown 0x4 opcode %x\n", opcode );
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
					case 0x9: { //swap.w rm, rn
						int dest = (opcode >> 8) & 0xF;
						int src = (opcode >> 4) & 0xF;
						context.gpr[ dest ] = context.gpr[ src ] >> 16;
						context.gpr[ dest ] |= context.gpr[ src ] << 16;
						break;
					}

					default: {
						printf( "Unknown 0x6 opcode %x\n", opcode );
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
		if( running ) {
			context.pc += 2;
		}
	}

	printf( " r0 %08x |  r1 %08x |  r2 %08x |  r3 %08x\n", context.gpr[ 0], context.gpr[ 1], context.gpr[ 2], context.gpr[ 3] );
	printf( " r4 %08x |  r5 %08x |  r6 %08x |  r7 %08x\n", context.gpr[ 4], context.gpr[ 5], context.gpr[ 6], context.gpr[ 7] );
	printf( " r8 %08x |  r9 %08x | r10 %08x | r11 %08x\n", context.gpr[ 8], context.gpr[ 9], context.gpr[10], context.gpr[11] );
	printf( "r12 %08x | r13 %08x | r14 %08x | r15 %08x\n", context.gpr[12], context.gpr[13], context.gpr[14], context.gpr[15] );
	printf( " pc %08x\n", context.pc );
}

