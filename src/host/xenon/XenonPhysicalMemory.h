#ifndef GEN7_HOST_XENON_XENONPHYSICALMEMORY_H
#define GEN7_HOST_XENON_XENONPHYSICALMEMORY_H

#include "Subsystem.h"
#include "PhysicalMemory.h"

namespace Gen7 {

class XenonPhysicalMemory : public NativePhysicalMemory, public Subsystem
{
private:
	static const int PAGE_SIZE = 1024;

	static const int RAM_REGION = 1;
	static const int SOC_REGION = 2;

	static const uint64_t BANNED_PHYS_BITS_MASK = 0xFFFFFCFF00000000UL;

	static const size_t DRAM_SIZE = 512 * 1024 * 1024;
	static const size_t SRAM_SIZE =         64 * 1024;
	static const size_t NAND_SIZE =  16 * 1024 * 1024;
	static const size_t BROM_SIZE =         32 * 1024;

	static const char * BROM_FILE_NAME;

	void * dram;
	void * sram;
	void * nand;
	void * brom;

	void LoadNand();
	void LoadBrom();

	void WriteRam16( uint32_t addr, uint16_t data );
	void WriteRam32( uint32_t addr, uint32_t data );
	void WriteSoc16( uint32_t addr, uint16_t data );
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

	XenonPhysicalMemory( MachineContext &context )
	  : Subsystem( context )
	  , dram( nullptr )
	  , sram( nullptr )
	  , nand( nullptr )
	  , brom( nullptr )
	{ }

	virtual ~XenonPhysicalMemory()
	{
		free( dram );
		free( sram );
		free( nand );
		free( brom );
	}
};

} //namespace Gen7

#endif //GEN7_HOST_XENON_XENONPHYSICALMEMORY_H

