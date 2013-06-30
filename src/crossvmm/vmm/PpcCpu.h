#ifndef GEN7_CROSSVMM_VMM_PPCCPU_H
#define GEN7_CROSSVMM_VMM_PPCCPU_H

#include "shared/Types.h"

class PpcCpu
{
private:
	Gen7::XenonPpcContext &context;

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

public:
	void Init();

	void Execute();

	PpcCpu( Gen7::XenonPpcContext &context )
	  : context( context )
	  , mmuContext( context )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_PPCCPU_H

