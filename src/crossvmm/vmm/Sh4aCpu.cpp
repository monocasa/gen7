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
		uint16_t *pc = reinterpret_cast<uint16_t*>( context.pc );

		switch( (*pc) >> 12 ) {
			case 0xE: {
				int32_t imm = Util::SignExtend<int32_t,8>( (*pc) & 0xFF );
				int reg = ((*pc) >> 8) & 0xF;
				context.gpr[ reg ] = imm;
				break;
			}

			default: {
				printf( "%08x : %04x : <UNKNOWN_OPCODE>\n", context.pc, *pc );
				running = false;
				break;
			}
		}
		context.pc += 2;
	}
}

