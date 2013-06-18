#ifndef GEN7_HOST_XENONPHYSICALMEMORY_H
#define GEN7_HOST_XENONPHYSICALMEMORY_H

#include "Subsystem.h"
#include "PhysicalMemory.h"

namespace Gen7 {

class XenonPhysicalMemory : public PhysicalMemory, public Subsystem
{
private:
	static const int PAGE_SIZE = 1024;

	static const uint64_t BANNED_PHYS_BITS_MASK = 0xFFFFFCFF00000000UL;

	void * ram;
	uint32_t ramSize;

	void WriteRam32( uint32_t addr, uint32_t data );
	void WriteSoc32( uint32_t addr, uint32_t data );

	uint32_t ReadRam32( uint32_t addr );
	uint32_t ReadSoc32( uint32_t addr );

public:
	virtual void Init( InitPhase phase );

	virtual const char * GetName() const
	{
		return "XenonPhysicalMemory";
	}

	virtual const char * GetShortName() const
	{
		return "XEPHYS";
	}

	uint32_t GetRamSize() const;

	virtual void WritePhys8( uint64_t addr, uint8_t data );

	virtual void WritePhys32( uint64_t addr, uint32_t data );
	virtual uint32_t ReadPhys32( uint64_t addr );

	virtual void WritePhys64( uint64_t addr, uint64_t data ) {
		uint32_t upper = data >> 32;
		uint32_t lower = data;

		WritePhys32( addr + 0, upper );
		WritePhys32( addr + 4, lower );
	}

	XenonPhysicalMemory( MachineContext &context )
	  : PhysicalMemory()
	  , Subsystem( context )
	  , ram( nullptr )
	{ }

	virtual ~XenonPhysicalMemory()
	{
		free( ram );
	}
};

} //namespace Gen7

#endif //GEN7_HOST_XENONPHYSICALMEMORY_H

