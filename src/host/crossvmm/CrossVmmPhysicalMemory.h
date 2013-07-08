#ifndef GEN7_HOST_CROSSVMM_CROSSVMMPHYSICALMEMORY_H
#define GEN7_HOST_CROSSVMM_CROSSVMMPHYSICALMEMORY_H

#include "PhysicalMemory.h"
#include "Subsystem.h"

#include <vector>

namespace Gen7 {

class CrossVmmPhysicalMemory : public PhysicalMemory, public Subsystem
{
private:
	static const size_t VMM_RAM_SIZE = 512 * 1024 * 1024;

	static const size_t PER_THREAD_START = 0x3000;

	void * ram;
	uint8_t  * ram8;
	uint32_t * ram32;

	NativePhysicalMemory &nativeMem;

public:
	void* GetPerThreadBase() {
		return reinterpret_cast<void*>( &ram8[ PER_THREAD_START ] );
	}

	virtual void WritePhys8( uint64_t addr, uint8_t data );

	virtual void WritePhys32( uint64_t addr, uint32_t data );
	virtual uint32_t ReadPhys32( uint64_t addr );

	virtual void WritePhys64( uint64_t addr, uint64_t data );

	virtual const char* GetName() const {
		return "CrossVmmPhysicalMemory";
	}

	virtual const char* GetShortName() const {
		return "VMPHYS";
	}

	virtual void Init( InitPhase phase );

	CrossVmmPhysicalMemory( MachineContext& context, NativePhysicalMemory &nativeMem )
	  : Subsystem( context )
	  , ram( nullptr )
	  , ram8( nullptr )
	  , ram32( nullptr )
	  , nativeMem( nativeMem )
	{ }

	virtual ~CrossVmmPhysicalMemory()
	{
		free( ram );
		// ram8 points to ram
		// ram32 points to ram
	}
};

} //namespace gen7

#endif //GEN7_HOST_CROSSVMM_CROSSVMMPHYSICALMEMORY_H

