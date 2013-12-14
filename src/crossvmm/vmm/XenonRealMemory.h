#ifndef GEN7_CROSSVMM_VMM_XENONREALMEMORY_H
#define GEN7_CROSSVMM_VMM_XENONREALMEMORY_H

class XenonRealMemory
{
private:
	static const uint64_t XENON_RAM_PHYS_BASE = 0x100000000UL;
	static const uint64_t XENON_SOC_PHYS_BASE = 0x200000000UL;

	static const uint64_t XENON_RAM_VIRT_BASE = 0xFFFFFFFD00000000UL;
	static const uint64_t XENON_SOC_VIRT_BASE = 0xFFFFFFFE00000000UL;

	uint64_t* ramPml2[4];
	uint64_t* socPml2[4];

	void MapUpperRealSpace();

public:
	uint64_t* GetRamPml2( int num ) {
		return ramPml2[ num ];
	}

	uint64_t* GetSocPml2( int num ) {
		return socPml2[ num ];
	}

	void Init();

	uint64_t GetVmmPhysForXenonPhys( uint64_t xenonPhys );
};

extern XenonRealMemory xenonReal;

#endif //GEN7_CROSSVMM_VMM_XENONREALMEMORY_H

