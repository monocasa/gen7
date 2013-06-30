#include "crossvmm/CrossVmmPhysicalMemory.h"
#include "sys/Exception.h"
#include "sys/Mem.h"

namespace Gen7 {

void CrossVmmPhysicalMemory::WritePhys8( uint64_t addr, uint8_t data )
{
	if( addr < VMM_RAM_SIZE ) {
		ram8[ addr ] = data;
	}
	else {
		throw Sys::Exception( "Implement CrossVmmPhysicalMemory::Write8( 0x%lx, 0x%x )", addr, data );
	}
}

void CrossVmmPhysicalMemory::WritePhys32( uint64_t addr, uint32_t data )
{
	//TODO: assert on top 7 nybbles not being zero
	int region = (addr >> 32) & 0xF;
	uint32_t regionOffset = (uint32_t)addr;

	//TODO:  make this branch unlikely
	if( VMM_REGION == region ) {
		if( addr < VMM_RAM_SIZE ) {
			ram32[ addr / sizeof(uint32_t) ] = data;
		}
		else {
			throw Sys::Exception( "Write to VMMPHYS%016lx(%08x)", addr, data );
		}
	}
	else {
		return nativeMem.WriteRegion32( region, regionOffset, data );
	}
}

uint32_t CrossVmmPhysicalMemory::ReadPhys32( uint64_t addr )
{
	//TODO: assert on top 7 nybbles not being zero
	int region = (addr >> 32) & 0xF;
	uint32_t regionOffset = (uint32_t)addr;

	//TODO:  make this branch unlikely
	if( VMM_REGION == region ) {
		if( addr < VMM_RAM_SIZE ) {
			return ram32[ addr / sizeof(uint32_t) ];
		}
		throw Sys::Exception( "Read from VMMPHYS%016lx", addr );
	}
	else {
		return nativeMem.ReadRegion32( region, regionOffset );
	}
}

void CrossVmmPhysicalMemory::WritePhys64( uint64_t addr, uint64_t data )
{
	throw Sys::Exception( "Implement CrossVmmPhysicalMemory::WritePhys64( 0x%lx, 0x%lx )", addr, data );
}

void CrossVmmPhysicalMemory::Init( InitPhase phase )
{
	switch( phase ) {
		case InitPhase::ALLOCATION: {
			ram = Sys::AllocatePageMem( VMM_RAM_SIZE );
			ram8 = reinterpret_cast<uint8_t*>( ram );
			ram32 = reinterpret_cast<uint32_t*>( ram );

			AddMemoryEntry( "WRAM",  ram, 0, 0x000000000, VMM_RAM_SIZE, false );

			break;
		}

		case InitPhase::APP_1: {
			//dependent on nativeMem::Init( ALLOCATION )
			for( const auto& nativeMemEntry : nativeMem.GetMemoryEntries() ) {
				memoryEntries.push_back( nativeMemEntry );
			}

			break;
		}

		default: {
			break;
		}
	}
}

} //namespace Gen7

