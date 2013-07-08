#ifndef GEN7_HOST_DC_DCPHYSICALMEMORY_H
#define GEN7_HOST_DC_DCPHYSICALMEMORY_H

#include "PhysicalMemory.h"
#include "Subsystem.h"

namespace Gen7 {

class DcPhysicalMemory : public NativePhysicalMemory, public Subsystem
{
private:
	static const int PHYS_REGION = 1;

	static const size_t BIOS_SIZE =  2 * 1024 * 1024;
	static const size_t DRAM_SIZE = 16 * 1024 * 1024;
	static const size_t VRAM_SIZE =  8 * 1024 * 1024;

	static const char * BIOS_FILE_NAME;

	void *bios;
	void *dram;
	void *vram;

	void LoadBios();

public:
	virtual void Init( InitPhase phase );

	virtual const char * GetName() const
	{
		return "DcPhysicalMemory";
	}

	virtual const char * GetShortName() const
	{
		return "DCPHYS";
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

	DcPhysicalMemory( MachineContext &context )
	  : Subsystem( context )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_DC_DCPHYSICALMEMORY_H

