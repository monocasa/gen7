#ifndef GEN7_CROSSVMM_VMM_PPCCPU_H
#define GEN7_CROSSVMM_VMM_PPCCPU_H

#include "Cpu.h"

#include "jit/powerpc/XenonCpuContext.h"
#include "jit/InterInstr.h"

#include <cstdio>

class PpcCpu : public Cpu
{
private:
	jit::XenonCpuContext &context;

	//Not real GPRs
	static const int GPR_LR    = 32;
	static const int GPR_CTR   = 33;
	static const int GPR_SPRG0 = 34;
	static const int GPR_SPRG1 = 35;
	static const int GPR_SPRG2 = 36;
	static const int GPR_SPRG3 = 37;

	//PowerPC SPRs
	static const int SPR_LR    = 8;
	static const int SPR_CTR   = 9;
	static const int SPR_SRR1  = 27;
	static const int SPR_SPRG0 = 272;
	static const int SPR_SPRG1 = 273;
	static const int SPR_SPRG2 = 274;
	static const int SPR_SPRG3 = 275;
	static const int SPR_HRMOR = 313;
	static const int SPR_LPCR  = 318;
	static const int SPR_LPIDR = 319;
	static const int SPR_HID6  = 1017;
	static const int SPR_PIR   = 1023;

	//Not actually real SPRs
	static const int SPR_MSR = 1024;

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

	int BuildIntermediateBranchConditional( jit::InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	int BuildIntermediateTable19( jit::InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	int BuildIntermediateSpecial( jit::InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	int BuildIntermediate( jit::InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );

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
	  : Cpu( &context.gpr[0] )
	  , context( context )
	  , mmuContext( context )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_PPCCPU_H

