#include "MemoryManager.h"
#include "XenonRealMemory.h"

#include <cstdio>

XenonRealMemory xenonReal;

void XenonRealMemory::MapUpperRealSpace()
{
	for( uint64_t i = 0; i < 4; i++ ) {
		pml2s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

		for( uint64_t j = 0; j < 512; j++ ) {
			uint64_t offset = (j * (2 * 1024 * 1024UL)) + (i * (1024 * 1024 * 1024UL));
			uint64_t physaddr = XENON_RAM_PHYS_BASE | offset;
			//printf( "mapping %lx\n", physaddr );
			pml2s[ i ][ j ] = physaddr | 0x83;
		}

		mm.SetPml2Page( pml2s[ i ], XENON_RAM_VIRT_BASE + (i * 1024 * 1024 * 1024UL ) );
	}

	for( uint64_t i = 0; i < 4; i++ ) {
		pml2s[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

		for( uint64_t j = 0; j < 512; j++ ) {
			uint64_t offset = (j * (2 * 1024 * 1024UL)) + (i * (1024 * 1024 * 1024UL));
			uint64_t physaddr = XENON_SOC_PHYS_BASE | offset;
			//printf( "mapping %lx\n", physaddr );
			pml2s[ i ][ j ] = physaddr | 0x83;
		}

		mm.SetPml2Page( pml2s[ i ], XENON_SOC_VIRT_BASE + (i * 1024 * 1024 * 1024UL ) );
	}
}

void XenonRealMemory::Init()
{
	MapUpperRealSpace();

	uint32_t *virt = reinterpret_cast<uint32_t*>( XENON_SOC_VIRT_BASE + 0x100 );
	uint64_t addr = 0x8000020000000100UL;
	int offset = 0;
	for( int i = 0; i < 20; i++ ) {
		printf( "0x%016lx : %08x\n", addr + (i * 4), virt[ i ] );
	}
}

