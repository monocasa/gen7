#include "dc/DcPhysicalMemory.h"
#include "sys/Mem.h"
#include "sys/File.h"

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

void DcPhysicalMemory::WritePhys8( uint64_t /*addr*/, uint8_t /*data*/ )
{
}

void DcPhysicalMemory::WritePhys32( uint64_t /*addr*/, uint32_t /*data*/ )
{
}

uint32_t DcPhysicalMemory::ReadPhys32( uint64_t /*addr*/ )
{
	return 0;
}

void DcPhysicalMemory::WriteRegion32( int /*region*/, uint32_t /*addr*/, uint32_t /*data*/ )
{
}

uint32_t DcPhysicalMemory::ReadRegion32( int /*region*/, uint32_t /*addr*/ )
{
	return 0;
}

} //namespace Gen7

