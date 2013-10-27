#ifndef GEN7_CROSSVMM_VMM_PPCCPU_H
#define GEN7_CROSSVMM_VMM_PPCCPU_H

#include "Cpu.h"
#include "InterInstr.h"

#include "shared/Types.h"

#include <cstdio>

class PpcCpu : public Cpu
{
private:
	Gen7::XenonPpcContext &context;

	static const int SPR_HID6 = 1017;

	class MmuContext
	{
	private:
		static const int NUM_HVREALMODE_PML3S = 4;

		static const int NUM_IR32_PML2S = 4;
		static const int NUM_IR32_PML1S = 1024;

		Gen7::XenonPpcContext &context;

		uint64_t* hvRealModePml3s[ NUM_HVREALMODE_PML3S ];  //PML3s for Hypervisor Real Mode

		uint64_t* ir32Pml3;                     //PML3 for instruction relocate/32 bit
		uint64_t* ir32Pml2s[ NUM_IR32_PML2S ]; //PML2s for instruction relocate/32 bit
		uint64_t* ir32Pml1s[ NUM_IR32_PML1S ]; //PML1s for instruction relocate/32 bit

	public:
		void Init();

		void MapFull();

		MmuContext( Gen7::XenonPpcContext &context )
		  : context( context )
		{ }

	} mmuContext;

	void DumpContext();

	int BuildIntermediateTable19( InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	int BuildIntermediateSpecial( InterInstr *intermediates, const uint32_t nativeInstr, uint64_t pc );
	int BuildIntermediate( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );

public:
	virtual void Init();

	virtual void Execute();

	virtual void SetPC( uint64_t pc ) {
		context.pc = pc;
	}

	virtual void SetGPR( int gpr, uint64_t newValue ) {
		if( gpr >= 0 && gpr <= 31 ) {
			context.gpr[ gpr ] = newValue;
			return;
		}
		if( gpr == 32 ) {
			context.ctr = newValue;
			return;
		}
		printf( "Unimplemented gpr %d set to 0x%lx\n", gpr, newValue );
	}

	virtual uint64_t ReadGPR( int gpr ) {
		if( gpr >= 0 && gpr <= 31 ) {
			return context.gpr[ gpr ];
		}

		printf( "Unimplemented gpr %d read from\n", gpr );
		return 0;
	}

	virtual bool SetSystemReg( int sysReg, uint64_t value );
	virtual bool ReadSystemReg( int sysReg, uint64_t &value );

	virtual bool InterpretProcessorSpecific( InterInstr &instr );

	PpcCpu( Gen7::XenonPpcContext &context )
	  : context( context )
	  , mmuContext( context )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_PPCCPU_H

