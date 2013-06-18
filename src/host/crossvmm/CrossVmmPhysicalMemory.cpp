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
	if( addr < VMM_RAM_SIZE ) {
		ram32[ addr / sizeof(uint32_t) ] = data;
	}
	else {
		throw Sys::Exception( "Implement CrossVmmPhysicalMemory::WritePhys32( 0x%lx, 0x%x )", addr, data );
	}
}

uint32_t CrossVmmPhysicalMemory::ReadPhys32( uint64_t addr )
{
	if( addr < VMM_RAM_SIZE ) {
		return ram32[ addr / sizeof(uint32_t) ];
	}
	else {
		throw Sys::Exception( "Implement CrossVmmPhysicalMemory::ReadPhys32( 0x%lx )", addr );
	}
}

void CrossVmmPhysicalMemory::WritePhys64( uint64_t addr, uint64_t data )
{
	throw Sys::Exception( "Implement CrossVmmPhysicalMemory::WritePhys64( 0x%lx, 0x%lx )", addr, data );
}

void CrossVmmPhysicalMemory::Init()
{
	ram = Sys::AllocatePageMem( VMM_RAM_SIZE );
	ram8 = reinterpret_cast<uint8_t*>( ram );
	ram32 = reinterpret_cast<uint32_t*>( ram );

	dram = Sys::AllocatePageMem( DRAM_SIZE );
	sram = Sys::AllocatePageMem( SRAM_SIZE );
	nand = Sys::AllocatePageMem( NAND_SIZE );
	brom = Sys::AllocatePageMem( BROM_SIZE );

	AddMemoryEntry( "WRAM",  ram, 0x000000000UL, VMM_RAM_SIZE, false );
	AddMemoryEntry( "DRAM", dram, 0x100000000UL,    DRAM_SIZE, false );
	AddMemoryEntry( "SRAM", sram, 0x1C0000000UL,    SRAM_SIZE, false );
	AddMemoryEntry( "NAND", nand, 0x1C8000000UL,    NAND_SIZE, true );
	AddMemoryEntry( "BROM", brom, 0x200000000UL,    BROM_SIZE, true );
}

} //namespace Gen7

