#ifndef GEN7_CROSSVMM_VMM_CPU_H
#define GEN7_CROSSVMM_VMM_CPU_H

#include <cstdint>

struct InterInstr;

class Cpu
{
public:
	virtual void Init() = 0;
	virtual void Execute() = 0;

	virtual void SetPC( uint64_t newPc ) = 0;
	virtual void SetGPR( int gpr, uint64_t newValue ) = 0;

	virtual uint64_t ReadGPR( int gpr ) = 0;

	virtual bool SetSystemReg( int sysReg, uint64_t value ) = 0;
	virtual bool ReadSystemReg( int sysReg, uint64_t &value ) = 0;

	virtual bool InterpretProcessorSpecific( InterInstr &instr ) = 0;

	bool InterpretIntermediate( InterInstr &instr );
};

#endif //GEN7_CROSSVMM_VMM_CPU_H

