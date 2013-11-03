#ifndef GEN7_LIBJIT_CPUINTERPRETER_H
#define GEN7_LIBJIT_CPUINTERPRETER_H

#include <cstdint>

namespace jit {

struct InterInstr;

class CpuInterpreter
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
	virtual void SetPC( uint64_t newPc ) = 0;

	virtual bool SetSystemReg( int sysReg, uint64_t value ) = 0;
	virtual bool ReadSystemReg( int sysReg, uint64_t &value ) = 0;

	virtual bool InterpretProcessorSpecific( jit::InterInstr &instr ) = 0;

	bool InterpretIntermediate( jit::InterInstr &instr );

	CpuInterpreter( void* gprPtr )
	  : GPR_PTR( reinterpret_cast<uint8_t*>( gprPtr ) )
	{ }
};

} //namespace jit

#endif //GEN7_LIBJIT_CPUINTERPRETER_H
