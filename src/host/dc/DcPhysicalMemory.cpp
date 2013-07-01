#include "dc/DcPhysicalMemory.h"
#include "sys/Mem.h"

namespace Gen7 {

void DcPhysicalMemory::Init( InitPhase /*phase*/ )
{
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

