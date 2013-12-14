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

	uint64_t ReadMem64( uint64_t addr ) {
		uint32_t *ptr = MakePtr<uint32_t>( addr );
		uint64_t ret = ((uint64_t)ptr[0]) << 32;
		ret |= ptr[1];

		return ret;
	}

	void WriteMem32( uint64_t addr, uint32_t value ) {
		*MakePtr<uint32_t>( addr ) = value;
	}

	void WriteMem64( uint64_t addr, uint64_t value ) {
		uint32_t *ptr = MakePtr<uint32_t>( addr );
		ptr[0] = value >> 32;
		ptr[1] = value;
	}
};

#endif //GEN7_CROSSVMM_VMM_THIRTYTWOBITMEMORYPOLICY_H

