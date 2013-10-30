#ifndef GEN7_CROSSVMM_VMM_CPU_H
#define GEN7_CROSSVMM_VMM_CPU_H

#include <cstdint>
#include <cstdio>

struct InterInstr;

class Cpu
{
protected:
	uint8_t * const GPR_PTR;

	uint32_t ReadGPR32( int offset ) {
		return *reinterpret_cast<uint32_t*>( &GPR_PTR[offset] );
	}

	uint64_t ReadGPR64( int offset ) {
		return *reinterpret_cast<uint64_t*>( &GPR_PTR[offset] );
	}

	void SetGPR32( int offset, uint32_t value ) {
		//printf( "Setting r%d(%d) to 0x%x\n", offset/8, offset, value );
		*reinterpret_cast<uint32_t*>( &GPR_PTR[offset] ) = value;
	}

	void SetGPR64( int offset, uint64_t value ) {
		//printf( "Setting r%d(%d) to 0x%lx\n", offset/8, offset, value );
		*reinterpret_cast<uint64_t*>( &GPR_PTR[offset] ) = value;
	}

public:
	virtual void Init() = 0;
	virtual void Execute() = 0;

	virtual void SetPC( uint64_t newPc ) = 0;

	virtual bool SetSystemReg( int sysReg, uint64_t value ) = 0;
	virtual bool ReadSystemReg( int sysReg, uint64_t &value ) = 0;

	virtual bool InterpretProcessorSpecific( InterInstr &instr ) = 0;

	bool InterpretIntermediate( InterInstr &instr );

	Cpu( void* gprPtr )
	  : GPR_PTR( reinterpret_cast<uint8_t*>( gprPtr ) )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_CPU_H

