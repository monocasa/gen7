#ifndef GEN7_CROSSVMM_VMM_THIRTYTWOBITMEMORYPOLICY_H
#define GEN7_CROSSVMM_VMM_THIRTYTWOBITMEMORYPOLICY_H

class ThirtyTwoBitMemoryPolicy
{
private:
	template<typename T>
	static T* MakePtr( uint64_t addr ) {
		return reinterpret_cast<T*>( addr & 0x00000000FFFFFFFFUL );
	}

public:
	uint32_t ReadMem32( uint64_t addr ) {
		return *MakePtr<uint32_t>( addr );
	}
};

#endif //GEN7_CROSSVMM_VMM_THIRTYTWOBITMEMORYPOLICY_H

