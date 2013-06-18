#ifndef GEN7_HOST_CROSSVMM_CROSSVMMPHYSICALMEMORY_H
#define GEN7_HOST_CROSSVMM_CROSSVMMPHYSICALMEMORY_H

#include "PhysicalMemory.h"

#include <vector>

namespace Gen7 {

class CrossVmmPhysicalMemory : public PhysicalMemory
{
public:
	struct MemoryEntry {
		const char *name;
		void *addr;
		uint64_t vmmLocation;
		size_t size;
		bool readOnly;
	};

private:
	static const size_t VMM_RAM_SIZE = 512 * 1024 * 1024; //0x800000000UL;

	void * ram;
	uint8_t  * ram8;
	uint32_t * ram32;

	std::vector<MemoryEntry> memoryEntries;

	void* dram;
	void* sram;
	void* nand;
	void* brom;

	static const int DRAM_SIZE = 512 * 1024 * 1024;
	static const int SRAM_SIZE =         64 * 1024;
	static const int NAND_SIZE =  16 * 1024 * 1024;
	static const int BROM_SIZE =         32 * 1024;

	void AddMemoryEntry( const char *name, void *addr, uint64_t vmmLocation, size_t size, bool readOnly )
	{
		MemoryEntry entry;

		entry.name = name;
		entry.addr = addr;
		entry.vmmLocation = vmmLocation;
		entry.size = size;
		entry.readOnly = readOnly;

		memoryEntries.push_back( entry );
	}

public:
	virtual void WritePhys8( uint64_t addr, uint8_t data );

	virtual void WritePhys32( uint64_t addr, uint32_t data );
	virtual uint32_t ReadPhys32( uint64_t addr );

	virtual void WritePhys64( uint64_t addr, uint64_t data );

	const std::vector<MemoryEntry>& GetMemoryEntries() {
		return memoryEntries;
	}

	void Init();

	CrossVmmPhysicalMemory()
	  : ram( nullptr )
	  , ram8( nullptr )
	  , ram32( nullptr )
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

