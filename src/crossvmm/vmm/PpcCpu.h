#ifndef GEN7_CROSSVMM_VMM_PPCCPU_H
#define GEN7_CROSSVMM_VMM_PPCCPU_H

#include "Cpu.h"

#include "jit/powerpc/PowerPCIntermediateBuilder.h"
#include "jit/powerpc/XenonCpuContext.h"
#include "jit/CpuInterpreter.h"
#include "jit/InterInstr.h"

#include <cstdio>

class PpcCpu : public Cpu, 
               public jit::CpuInterpreter, 
               private jit::PowerPCIntermediateBuilder
{
private:
	jit::XenonCpuContext &context;

	class MmuContext
	{
	private:
		static const int NUM_HVREALMODE_PML3S = 4;

		static const int NUM_IR32_PML2S = 4;
		static const int NUM_IR32_PML1S = 1024;

		jit::XenonCpuContext &context;

		uint64_t* hvRealModePml3s[ NUM_HVREALMODE_PML3S ];  //PML3s for Hypervisor Real Mode

		uint64_t* ir32Pml3;                     //PML3 for instruction relocate/32 bit
		uint64_t* ir32Pml2s[ NUM_IR32_PML2S ]; //PML2s for instruction relocate/32 bit
		uint64_t* ir32Pml1s[ NUM_IR32_PML1S ]; //PML1s for instruction relocate/32 bit

	public:
		void Init();

		void MapFull();

		MmuContext( jit::XenonCpuContext &context )
		  : context( context )
		{ }

	} mmuContext;

	void DumpContext();

	//int BuildIntermediateBranchConditional( jit::InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	//int BuildIntermediateTable19( jit::InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	//int BuildIntermediateSpecial( jit::InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	//int BuildIntermediate( jit::InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );

public:
	virtual void Init();

	virtual void Execute();

	virtual void SetPC( uint64_t pc ) {
		context.pc = pc;
	}

	virtual bool SetSystemReg( int sysReg, uint64_t value );
	virtual bool ReadSystemReg( int sysReg, uint64_t &value );

	virtual bool InterpretProcessorSpecific( jit::InterInstr &instr );

	PpcCpu( jit::XenonCpuContext &context )
	  : CpuInterpreter( &context.gpr[0] )
	  , context( context )
	  , mmuContext( context )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_PPCCPU_H

