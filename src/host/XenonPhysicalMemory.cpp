#include "sys/File.h"
#include "sys/Exception.h"
#include "sys/Mem.h"
#include "MachineContext.h"
#include "XenonPhysicalMemory.h"

#include <cassert>
#include <cstdio>

namespace Gen7 {

const char * XenonPhysicalMemory::BROM_FILE_NAME = "/opt/gen7/roms/xenon/1bl.bin";

void XenonPhysicalMemory::LoadNand()
{
	nand = Sys::AllocatePageMem( NAND_SIZE );

	memset( nand, 0xFF, NAND_SIZE );
}

void XenonPhysicalMemory::LoadBrom()
{
	brom = Sys::AllocatePageMem( BROM_SIZE );
	uint32_t* brom32 = (uint32_t*)brom;

	Sys::File bromFile( BROM_FILE_NAME );

	if( !bromFile.IsOpen() ) {
		throw Sys::Exception( "Unable to open xenon 1bl (%s)", BROM_FILE_NAME );
	}

	if( bromFile.Size() != BROM_SIZE ) {
		throw Sys::Exception( "1bl is the wrong size (%ld bytes)", bromFile.Size() );
	}

	// Byte swap
	bromFile.SetEndian( Sys::File::BIG );

	for( size_t i = 0; i < (BROM_SIZE / sizeof(uint32_t)); i++ ) {
		brom32[ i ] = bromFile.Read32();
	}
}

void XenonPhysicalMemory::WriteRam32( uint32_t addr, uint32_t data )
{
	if( addr < ramSize ) {
		((uint32_t*)dram)[ addr / sizeof(uint32_t) ] = data;
	}
	else {
		throw Sys::Exception( "Write of %08x to unknown RAM address %08x", data, addr );
	}
}

void XenonPhysicalMemory::WriteSoc32( uint32_t addr, uint32_t data )
{
	throw Sys::Exception( "Write of %08x to unknown SoC address %08x", data, addr );
}

uint32_t XenonPhysicalMemory::ReadRam32( uint32_t addr )
{
	if( addr < ramSize ) {
		return ((uint32_t*)dram)[ addr / sizeof(uint32_t) ];
	}
	else {
		throw Sys::Exception( "Read from unknown RAM address %08x", addr );
	}
}

uint32_t XenonPhysicalMemory::ReadSoc32( uint32_t addr )
{
	if( addr < BROM_SIZE ) {
		return ((uint32_t*)brom)[ addr / sizeof(uint32_t) ];
	}
	throw Sys::Exception( "Read from unknown SoC address %08x", addr );
}

void XenonPhysicalMemory::Init( InitPhase phase )
{
	switch( phase ) {
		case InitPhase::ALLOCATION: {
			ramSize = GetRamSize();

			DPRINT( "Allocating %dMiB for DRAM\n", ramSize / 1024 / 1024);

			dram = Sys::AllocatePageMem( ramSize );

			sram = Sys::AllocatePageMem( SRAM_SIZE );

			LoadNand();

			LoadBrom();

			AddMemoryEntry( "DRAM", dram, RAM_REGION, 0x00000000,   ramSize, false );
			AddMemoryEntry( "SRAM", sram, RAM_REGION, 0xC0000000, SRAM_SIZE, false );
			AddMemoryEntry( "NAND", nand, RAM_REGION, 0xC8000000, NAND_SIZE, true );
			AddMemoryEntry( "BROM", brom, SOC_REGION, 0x00000000, BROM_SIZE, true );
		}
		break;

		default:
		break;
	}
}

uint32_t XenonPhysicalMemory::GetRamSize() const
{
	switch( context.GetConsoleType() ) {
		case ConsoleType::RETAIL:
		case ConsoleType::DEVKIT: {
			return 512 * 1024 * 1024;
		}

		case ConsoleType::ADK: {
			return 1024 * 1024 * 1024;
		}

		default: {
			throw Sys::Exception( "Unknown Console Type %d in PhysicalMemory::GetRamSize()", context.GetConsoleType() );
		}
	}
}

void XenonPhysicalMemory::WritePhys8( uint64_t addr, uint8_t data )
{
	throw Sys::Exception( "Implement XenonPhysicalMemory::WritePhys8( addr=0x%lx, data=%x )", addr, data );
}

void XenonPhysicalMemory::WritePhys32( uint64_t addr, uint32_t data )
{
	assert( 0 == (addr & BANNED_PHYS_BITS_MASK) );  // Check for bits that shouldn't ever be turned on
	assert( 0 == (addr & 3 ) );  // Check that we're aligned

	switch( (addr >> 40) & 3 ) {
		case 0:  // Traditional view of RAM
			WriteRam32( (uint32_t)addr, data );
			break;

		case 1:  // Encrypted and hashed view of RAM
			WriteRam32( (uint32_t)addr, data );
			break;

		case 2:  // SoC space
			WriteSoc32( (uint32_t)addr, data );
			break;

		case 3:  // Encrypted view of RAM
			WriteRam32( (uint32_t)addr, data );
			break;
	}
}

uint32_t XenonPhysicalMemory::ReadPhys32( uint64_t addr )
{
	assert( 0 == (addr & BANNED_PHYS_BITS_MASK) );  // Check for bits that shouldn't ever be turned on
	assert( 0 == (addr & 3 ) );  // Check that we're aligned

	switch( (addr >> 40) & 3 ) {
		case 0:  // Traditional view of RAM
			return ReadRam32( (uint32_t)addr );

		case 1:  // Encrypted and hashed view of RAM
			return ReadRam32( (uint32_t)addr );

		case 2:  // SoC space
			return ReadSoc32( (uint32_t)addr );

		case 3:  // Encrypted view of RAM
			return ReadRam32( (uint32_t)addr );

		default:
			throw Sys::Exception( "We should never get here:  ReadPhys32( %016lx )", addr );
	}
}

void XenonPhysicalMemory::WriteRegion32( int region, uint32_t addr, uint32_t data )
{
	assert( region == RAM_REGION || region == SOC_REGION );

	switch( region ) {
		case RAM_REGION: {
			WriteRam32( addr, data );
			return;
		}

		case SOC_REGION: {
			WriteSoc32( addr, data );
			return;
		}
	}
}

uint32_t XenonPhysicalMemory::ReadRegion32( int region, uint32_t addr )
{
	switch( region ) {
		case RAM_REGION: {
			return ReadRam32( addr );
		}

		case SOC_REGION: {
			return ReadSoc32( addr );
		}

		default: {
			throw Sys::Exception( "Read from unmapped in XenonPhysicalMemory::ReadRegion32( region: %d, addr: %08x )", region, addr );
		}
	}
}

} //namespace Gen7

