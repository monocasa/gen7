#include "MemoryManager.h"
#include "PpcCpu.h"
#include "XenonRealMemory.h"

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

	uint64_t virtAddr = 0x0000000000000000UL;

	mm.SetLowerPml3( hvRealModePml3s[ 0 ], 0x0000000000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 1 ], 0x0000010000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 2 ], 0x0000020000000000UL );
	mm.SetLowerPml3( hvRealModePml3s[ 3 ], 0x0000030000000000UL );
}

void PpcCpu::Init()
{
	mmuContext.Init();
	mmuContext.MapFull();
}

void PpcCpu::Execute()
{
	uint32_t* pc = (uint32_t*)(context.pc);

	for( int i = 0; i < 16; i++ ) {
		printf( "%p : 0x%08x\n", pc, *pc );
		pc++;
	}

	uint32_t* bootRom = (uint32_t*)( 0x0000020000000100 );
	for( int i = 0; i < 16; i++ ) {
		printf( "%p : 0x%08x\n", bootRom, *bootRom );
		bootRom++;
	}
}

