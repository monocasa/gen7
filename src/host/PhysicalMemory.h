#ifndef GEN7_HOST_PHYSICALMEMORY_H
#define GEN7_HOST_PHYSICALMEMORY_H

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace gen7 {

class PhysicalMemory
{
public:
	struct MemoryEntry {
		const char *name;
		void *addr;
		int memRegion;
		uint32_t regionOffset;
		size_t size;
		bool readOnly;
	};

	static const int VMM_REGION = 0;

protected:
	void AddMemoryEntry( const char *name, void *addr, int memRegion, uint32_t regionOffset, size_t size, bool readOnly )
	{
		MemoryEntry entry;

		entry.name = name;
		entry.addr = addr;
		entry.memRegion = memRegion;
		entry.regionOffset = regionOffset;
		entry.size = size;
		entry.readOnly = readOnly;

		memoryEntries.push_back( entry );
	}

	std::vector<MemoryEntry> memoryEntries;

public:	
	virtual void WritePhys8( uint64_t addr, uint8_t data ) = 0;

	virtual void WritePhys16( uint64_t addr, uint16_t data ) = 0;

	virtual void WritePhys32( uint64_t addr, uint32_t data ) = 0;
	virtual uint32_t ReadPhys32( uint64_t addr ) = 0;

	virtual void WritePhys64( uint64_t addr, uint64_t data ) = 0;

	std::vector<MemoryEntry>& GetMemoryEntries() {
		return memoryEntries;
	}
};

class NativePhysicalMemory : public PhysicalMemory
{
public:
	virtual void WriteRegion16( int region, uint32_t addr, uint16_t data ) = 0;

	virtual void WriteRegion32( int region, uint32_t addr, uint32_t data ) = 0;
	virtual uint32_t ReadRegion32( int region, uint32_t addr ) = 0;
};

} //namespace gen7

#endif //GEN7_HOST_PHYSICALMEMORY_H

