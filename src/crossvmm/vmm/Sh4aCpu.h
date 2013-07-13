#ifndef GEN7_CROSSVMM_VMM_SH4ACPU_H
#define GEN7_CROSSVMM_VMM_SH4ACPU_H

#include "Cpu.h"
#include "MemoryManager.h"

#include "shared/Types.h"

struct InterruptRegs;

class MemoryEmulator
{
public:
	virtual void Write8( uint64_t addr, uint8_t data ) = 0;

	virtual void Write16( uint64_t addr, uint16_t data ) = 0;
	virtual uint16_t Read16( uint64_t addr ) = 0;

	virtual void Write32( uint64_t addr, uint32_t data ) = 0;
	virtual uint32_t Read32( uint64_t addr ) = 0;
};

class Sh4aCpu : public Cpu
{
private:
	static const uint32_t T_BIT = 0x000000001;

	Gen7::Sh4aContext &context;

	class MmuContext : public PageFaultHandler, MemoryEmulator
	{
	private:
		static const int NUM_PML2S = 4;
		static const int NUM_PML1S = 1024;

		Gen7::Sh4aContext &context;

		uint64_t* pml3;
		uint64_t* pml2s[ NUM_PML2S ];
		uint64_t* pml1s[ NUM_PML1S ];

		uint64_t* ccnPml1;
		uint32_t* ccnPage;

		uint64_t* bscPml1;
		uint32_t* bscPage;

		uint32_t* sdmrPage;

		void P4Access( uint64_t addr, InterruptRegs *regs );

	public:
		void Init();

		void MapFull();

		virtual void OnPageFault( PageFaultReason reason, uint64_t addr, InterruptRegs *regs );

		virtual void Write8( uint64_t addr, uint8_t data );

		virtual void Write16( uint64_t addr, uint16_t data );
		virtual uint16_t Read16( uint64_t addr );

		virtual void Write32( uint64_t addr, uint32_t data );
		virtual uint32_t Read32( uint64_t addr );

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

