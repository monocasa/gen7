#ifndef GEN7_HOST_WII_WIIPHYSICALMEMORY_H
#define GEN7_HOST_WII_WIIPHYSICALMEMORY_H

#include "PhysicalMemory.h"
#include "Subsystem.h"

#include <cstdint>

namespace gen7 {

class WiiPhysicalMemory : public NativePhysicalMemory, public Subsystem
{
private:
	static const int PHYS_REGION = 1;

	static const size_t MEM1_SIZE = 24 * 1024 * 1024;
	static const size_t MEM2_SIZE = 64 * 1024 * 1024;

	static const uint32_t MEM1_BASE = 0x00000000;
	static const uint32_t MEM2_BASE = 0x10000000;

	void *mem1;
	void *mem2;

public:
	virtual void Init( InitPhase phase );

	virtual const char * GetName() const
	{
		return "WiiPhysicalMemory";
	}

	virtual const char * GetShortName() const
	{
		return "WIPHYS";
	}

	virtual void WritePhys8( uint64_t addr, uint8_t data );

	virtual void WritePhys16( uint64_t addr, uint16_t data );

	virtual void WritePhys32( uint64_t addr, uint32_t data );
	virtual uint32_t ReadPhys32( uint64_t addr );

	virtual void WritePhys64( uint64_t addr, uint64_t data ) {
		uint32_t upper = data >> 32;
		uint32_t lower = data;

		WritePhys32( addr + 0, upper );
		WritePhys32( addr + 4, lower );
	}

	virtual void WriteRegion16( int region, uint32_t addr, uint16_t data );

	virtual void WriteRegion32( int region, uint32_t addr, uint32_t data );
	virtual uint32_t ReadRegion32( int region, uint32_t addr );

	WiiPhysicalMemory( MachineContext &context )
	  : Subsystem( context )
	{ }
};

}

#endif //GEN7_HOST_WII_WIIPHYSICALMEMORY_H

