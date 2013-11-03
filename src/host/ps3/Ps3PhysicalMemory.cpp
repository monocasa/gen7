#include "ps3/Ps3PhysicalMemory.h"
#include "sys/Mem.h"

namespace gen7 {

void Ps3PhysicalMemory::Init( InitPhase /*phase*/ )
{
//	switch( phase ) {
//		case InitPhase::ALLOCATION: {
//			xram = Sys::AllocatePageMem( XRAM_SIZE );
//
//			vram = Sys::AllocatePageMem( VRAM_SIZE );
//
//			for( int i = 0; i < NUM_SPES; i++ ) {
//				speLocalRam[i] = Sys::AllocatePageMem( SPELOCALRAM_SIZE );
//			}
//		}
//	}
}

void Ps3PhysicalMemory::WritePhys8( uint64_t /*addr*/, uint8_t /*data*/ )
{
}

void Ps3PhysicalMemory::WritePhys16( uint64_t /*addr*/, uint16_t /*data*/ )
{
}

void Ps3PhysicalMemory::WritePhys32( uint64_t /*addr*/, uint32_t /*data*/ )
{
}

uint32_t Ps3PhysicalMemory::ReadPhys32( uint64_t /*addr*/ )
{
	return 0;
}

void Ps3PhysicalMemory::WriteRegion16( int /*region*/, uint32_t /*addr*/, uint16_t /*data*/ )
{
}

void Ps3PhysicalMemory::WriteRegion32( int /*region*/, uint32_t /*addr*/, uint32_t /*data*/ )
{
}

uint32_t Ps3PhysicalMemory::ReadRegion32( int /*region*/, uint32_t /*addr*/ )
{
	return 0;
}

} //namespace gen7

