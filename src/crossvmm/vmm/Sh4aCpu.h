#ifndef GEN7_CROSSVMM_VMM_SH4ACPU_H
#define GEN7_CROSSVMM_VMM_SH4ACPU_H

#include "Cpu.h"

#include "shared/Types.h"

class Sh4aCpu : public Cpu
{
private:
	Gen7::Sh4aContext &context;

	class MmuContext
	{
	private:
		static const int NUM_PML2S = 4;
		static const int NUM_PML1S = 1024;

		Gen7::Sh4aContext &context;

		uint64_t* pml3;
		uint64_t* pml2s[ NUM_PML2S ];
		uint64_t* pml1s[ NUM_PML1S ];

	public:
		void Init();

		void MapFull();

		MmuContext( Gen7::Sh4aContext &context )
		  : context( context )
		{ }
	} mmu;

	void DumpState();

public:
	virtual void Init();

	virtual void Execute();

	Sh4aCpu( Gen7::Sh4aContext &context )
	  : context( context )
	  , mmu( context )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_SH4ACPU_H

