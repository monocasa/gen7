#include "MemoryManager.h"
#include "XenonRealMemory.h"

#include <cstdio>

XenonRealMemory xenonReal;

void XenonRealMemory::MapUpperRealSpace()
{
	for( uint64_t i = 0; i < 4; i++ ) {
		ramPml2[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

		for( uint64_t j = 0; j < 512; j++ ) {
			uint64_t offset = (j * (2 * 1024 * 1024UL)) + (i * (1024 * 1024 * 1024UL));
			uint64_t physaddr = XENON_RAM_PHYS_BASE | offset;
			//printf( "mapping %lx\n", physaddr );
			ramPml2[ i ][ j ] = physaddr | 0x83;
		}

		mm.SetPml2Page( ramPml2[ i ], XENON_RAM_VIRT_BASE + (i * 1024 * 1024 * 1024UL ) );
	}

	for( uint64_t i = 0; i < 4; i++ ) {
		socPml2[ i ] = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

		for( uint64_t j = 0; j < 512; j++ ) {
			uint64_t offset = (j * (2 * 1024 * 1024UL)) + (i * (1024 * 1024 * 1024UL));
			uint64_t physaddr = XENON_SOC_PHYS_BASE | offset;
			//printf( "mapping %lx\n", physaddr );
			socPml2[ i ][ j ] = physaddr | 0x83;
		}

		mm.SetPml2Page( socPml2[ i ], XENON_SOC_VIRT_BASE + (i * 1024 * 1024 * 1024UL ) );
	}
}

void XenonRealMemory::Init()
{
	MapUpperRealSpace();
}

uint64_t XenonRealMemory::GetVmmPhysForXenonPhys( uint64_t xenonPhys )
{
	const int region = (xenonPhys >> 40) & 0x3;
	const uint64_t regionOffset = xenonPhys & 0x00000000FFFFFFFFUL;
	switch( region ) {
		default:
		case 0: //normal ram
			return regionOffset + XENON_RAM_PHYS_BASE;

		case 1: //encrypted/hashed ram
			return regionOffset + XENON_RAM_PHYS_BASE;

		case 2: //soc space
			return regionOffset + XENON_SOC_PHYS_BASE;

		case 3: //encrypted ram
			return regionOffset + XENON_RAM_PHYS_BASE;
	}
}

