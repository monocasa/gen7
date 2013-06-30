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
	struct XenonUartRegs {
		uint32_t txData;
		uint32_t rxData;
		uint32_t status;
		uint32_t config;
	};

	volatile XenonUartRegs* uart = reinterpret_cast<volatile XenonUartRegs*>( 0x00000200EA001010 );
	uart->config = 0xE6010000;  // Set to 115200, 8, N, 1

	const char * str = "Hello Xenon UART";

	while( *str ) {
		while ( !(uart->status & 0x02000000) );
		uart->txData = ((*str) % 0xFF) << 24;
		str++;
	}
}

