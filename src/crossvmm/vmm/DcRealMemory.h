#ifndef GEN7_CROSSVMM_VMM_DCREALMEMORY_H
#define GEN7_CROSSVMM_VMM_DCREALMEMORY_H

#include <cstdint>

class DcRealMemory
{
private:
	static const uint64_t DC_PHYS_BASE = 0x100000000UL;

	static const uint64_t DC_VIRT_BASE = 0xFFFFFFFD00000000UL;

	uint64_t* physPml2;

	void MapUpperRealSpace();

public:
	uint64_t* GetPml2() {
		return physPml2;
	}

	void Init();
};

extern DcRealMemory dcReal;

#endif //GEN7_CROSSVMM_VMM_DCREALMEMORY_H

