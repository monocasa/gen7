#include "sys/Exception.h"
#include "MachineContext.h"
#include "XenonPhysicalMemory.h"

#include <cassert>
#include <cstdio>

namespace Gen7 {

void XenonPhysicalMemory::WriteRam32( uint32_t addr, uint32_t data )
{
	if( addr < ramSize ) {
		((uint32_t*)ram)[ addr / sizeof(uint32_t) ] = data;
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
		return ((uint32_t*)ram)[ addr / sizeof(uint32_t) ];
	}
	else {
		throw Sys::Exception( "Read from unknown RAM address %08x", addr );
	}
}

uint32_t XenonPhysicalMemory::ReadSoc32( uint32_t addr )
{
	throw Sys::Exception( "Read from unknown SoC address %08x", addr );
}

void XenonPhysicalMemory::Init( InitPhase phase )
{
	switch( phase ) {
		case InitPhase::ALLOCATION: {
			ramSize = GetRamSize();

			DPRINT( "Allocating %dMiB for DRAM\n", ramSize / 1024 / 1024);

			if( 0 != posix_memalign( &ram, PAGE_SIZE,  ramSize ) ) {
				throw Sys::Exception( "Unable to allocate %d page aligned bytes for DRAM", ramSize );
			}
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

} //namespace Gen7

