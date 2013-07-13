#include "dc/DcPhysicalMemory.h"
#include "sys/Mem.h"
#include "sys/File.h"

#include <cassert>

namespace Gen7 {

const char * DcPhysicalMemory::BIOS_FILE_NAME = "/opt/gen7/roms/dc/bios.bin";

void DcPhysicalMemory::LoadBios()
{
	bios = Sys::AllocatePageMem( BIOS_SIZE );
	uint32_t* bios32 = (uint32_t*)bios;

	Sys::File biosFile( BIOS_FILE_NAME );

	if( !biosFile.IsOpen() ) {
		throw Sys::Exception( "Unable to open dreamcast bios (%s)", BIOS_FILE_NAME );
	}

	if( biosFile.Size() != BIOS_SIZE ) {
		throw Sys::Exception( "dreamcast bios is the wrong size (%ld bytes)", biosFile.Size() );
	}

	for( size_t i = 0; i < (BIOS_SIZE / sizeof(uint32_t)); i++ ) {
		bios32[ i ] = biosFile.Read32();
	}
}

void DcPhysicalMemory::Init( InitPhase phase )
{
	switch( phase ) {
		case InitPhase::ALLOCATION: {
			LoadBios();

			dram = Sys::AllocatePageMem( DRAM_SIZE );

			vram = Sys::AllocatePageMem( VRAM_SIZE );

			AddMemoryEntry( "BIOS", bios, PHYS_REGION, 0x00000000, BIOS_SIZE, true );
			AddMemoryEntry( "DRAM", dram, PHYS_REGION, 0x0C000000, DRAM_SIZE, false );
			AddMemoryEntry( "VRAM", vram, PHYS_REGION, 0x10000000, VRAM_SIZE, false );

			break;
		}

		default: {
			break;
		}
	}
}

void DcPhysicalMemory::WritePhys8( uint64_t addr, uint8_t data )
{
	throw Sys::Exception( "Implement DcPhysicalMemory::WritePhys8( addr=%08x, data=%02x )", addr, data );
}

void DcPhysicalMemory::WritePhys16( uint64_t addr, uint16_t data )
{
	throw Sys::Exception( "Implement DcPhysicalMemory::WritePhys32( addr=%08x, data=%04x )", addr, data );
}

void DcPhysicalMemory::WritePhys32( uint64_t addr, uint32_t data )
{
	throw Sys::Exception( "Implement DcPhysicalMemory::WritePhys32( addr=%08x, data=%08x )", addr, data );
}

uint32_t DcPhysicalMemory::ReadPhys32( uint64_t addr )
{
	throw Sys::Exception( "Implement DcPhysicalMemory::ReadPhys32( addr=%08x )", addr );
}

void DcPhysicalMemory::WriteRegion16( int region, uint32_t addr, uint16_t data )
{
	assert( 1 == region );

	WritePhys16( addr, data );
}

void DcPhysicalMemory::WriteRegion32( int region, uint32_t addr, uint32_t data )
{
	assert( 1 == region );

	WritePhys32( addr, data );
}

uint32_t DcPhysicalMemory::ReadRegion32( int region, uint32_t addr )
{
	assert( 1 == region );

	return ReadPhys32( addr );
}

} //namespace Gen7

