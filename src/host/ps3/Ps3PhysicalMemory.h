#ifndef GEN7_HOST_PS3_PS3PHYSICALMEMORY_H
#define GEN7_HOST_PS3_PS3PHYSICALMEMORY_H

#include "PhysicalMemory.h"
#include "Subsystem.h"

namespace Gen7 {

class Ps3PhysicalMemory : public NativePhysicalMemory, public Subsystem
{
private:

public:
	virtual void Init( InitPhase phase );

	virtual const char * GetName() const
	{
		return "Ps3PhysicalMemory";
	}

	virtual const char * GetShortName() const
	{
		return "P3PHYS";
	}

	virtual void WritePhys8( uint64_t addr, uint8_t data );

	virtual void WritePhys32( uint64_t addr, uint32_t data );
	virtual uint32_t ReadPhys32( uint64_t addr );

	virtual void WritePhys64( uint64_t addr, uint64_t data ) {
		uint32_t upper = data >> 32;
		uint32_t lower = data;

		WritePhys32( addr + 0, upper );
		WritePhys32( addr + 4, lower );
	}

	virtual void WriteRegion32( int region, uint32_t addr, uint32_t data );
	virtual uint32_t ReadRegion32( int region, uint32_t addr );

	Ps3PhysicalMemory( MachineContext &context )
	  : Subsystem( context )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_PS3_PS3PHYSICALMEMORY_H

