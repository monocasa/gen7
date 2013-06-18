#ifndef GEN7_HOST_PHYSICALMEMORY_H
#define GEN7_HOST_PHYSICALMEMORY_H

#include <cstdint>
#include <cstdlib>

namespace Gen7 {

class PhysicalMemory
{
public:
	virtual void WritePhys8( uint64_t addr, uint8_t data ) = 0;

	virtual void WritePhys32( uint64_t addr, uint32_t data ) = 0;
	virtual uint32_t ReadPhys32( uint64_t addr ) = 0;

	virtual void WritePhys64( uint64_t addr, uint64_t data ) = 0;
};

} //namespace Gen7

#endif //GEN7_HOST_PHYSICALMEMORY_H

