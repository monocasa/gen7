#include "DcRealMemory.h"
#include "MemoryManager.h"

DcRealMemory dcReal;

void DcRealMemory::MapUpperRealSpace()
{
	physPml2 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

	for( int i = 0; i < 256; i++ ) {
		uint64_t offset = i * (2 * 1024 * 1024UL);
		uint64_t physaddr = DC_PHYS_BASE | offset;
		physPml2[ i ] = physaddr | 0x83;
	}

	for( int i = 256; i < 512; i++ ) {
		physPml2[ i ] = 0;
	}

	mm.SetPml2Page( physPml2, DC_VIRT_BASE );
}

void DcRealMemory::Init()
{
	MapUpperRealSpace();
}

