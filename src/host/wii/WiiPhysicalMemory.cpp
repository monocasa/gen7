#include "wii/WiiPhysicalMemory.h"
#include "sys/Exception.h"
#include "sys/Mem.h"

#include "util/Compilers.h"

namespace gen7 {

void WiiPhysicalMemory::Init( InitPhase phase )
{
	switch( phase ) {
		case InitPhase::ALLOCATION: {
			mem1 = sys::AllocatePageMem( MEM1_SIZE );
			mem2 = sys::AllocatePageMem( MEM2_SIZE );

			AddMemoryEntry( "MEM1", mem1, PHYS_REGION, MEM1_BASE, MEM1_SIZE, false );
			AddMemoryEntry( "MEM2", mem2, PHYS_REGION, MEM2_BASE, MEM2_SIZE, false );

			break;
		}

		default: {
			break;
		}
	}
}

void WiiPhysicalMemory::WritePhys8( uint64_t addr, uint8_t data )
{
	UNUSED( addr );
	UNUSED( data );
}

void WiiPhysicalMemory::WritePhys16( uint64_t addr, uint16_t data )
{
	UNUSED( addr );
	UNUSED( data );
}

void WiiPhysicalMemory::WritePhys32( uint64_t addr, uint32_t data )
{
	if( (addr > MEM1_BASE) && (addr < (MEM1_BASE + MEM1_SIZE)) ) {
		uint32_t *ptr = (uint32_t*)( mem1 );
		ptr[ addr / sizeof(uint32_t) ] = data;
	}
	else {
		throw sys::Exception( "WiiPhysicalMemory::WritePhys32( addr=%lx, data=%08x )\n", addr, data );
	}
}

uint32_t WiiPhysicalMemory::ReadPhys32( uint64_t addr )
{
	throw sys::Exception( "WiiPhysicalMemory::ReadPhys32( addr=%lx )\n", addr );
}

void WiiPhysicalMemory::WriteRegion16( int region, uint32_t addr, uint16_t data )
{
	UNUSED( region );
	UNUSED( addr );
	UNUSED( data );
}

void WiiPhysicalMemory::WriteRegion32( int region, uint32_t addr, uint32_t data )
{
	UNUSED( region );
	UNUSED( addr );
	UNUSED( data );
}

uint32_t WiiPhysicalMemory::ReadRegion32( int region, uint32_t addr )
{
	if( 1 != region ) {
		throw sys::Exception( "WiiPhysicalMemory::ReadRegion32( region=%d, addr=%lx )\n", region, addr );
	}
	return ReadPhys32( addr );
}

} //namespace gen7

