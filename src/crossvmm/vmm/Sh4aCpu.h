#ifndef GEN7_CROSSVMM_VMM_SH4ACPU_H
#define GEN7_CROSSVMM_VMM_SH4ACPU_H

#include "Cpu.h"
#include "MemoryManager.h"
#include "ThirtyTwoBitMemoryPolicy.h"

#include "jit/sh4a/Sh4aCpuContext.h"
#include "jit/CpuInterpreter.h"
#include "jit/InterInstr.h"

#include <cstdio>

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

class Sh4aCpu : public Cpu, public jit::CpuInterpreter<ThirtyTwoBitMemoryPolicy>
{
private:
	static const uint32_t SR_T_BIT  = 0x00000001;
	static const uint32_t SR_S_BIT  = 0x00000002;
	static const uint32_t SR_IMASK  = 0x000000F0;
	static const uint32_t SR_Q_BIT  = 0x00000100;
	static const uint32_t SR_M_BIT  = 0x00000200;
	static const uint32_t SR_FD_BIT = 0x00008000;
	static const uint32_t SR_BL_BIT = 0x10000000;
	static const uint32_t SR_RB_BIT = 0x20000000;
	static const uint32_t SR_MD_BIT = 0x40000000;
	static const uint32_t SR_ALL    = 0x700083F3;

	static const uint32_t FPSCR_SZ_BIT = 0x00100000;

	jit::Sh4aCpuContext &context;

	class MmuContext : public PageFaultHandler, MemoryEmulator
	{
	private:
		static const int NUM_PML2S = 4;
		static const int NUM_PML1S = 1024;

		jit::Sh4aCpuContext &context;
		Sh4aCpu &cpu;

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

		MmuContext( jit::Sh4aCpuContext &context, Sh4aCpu &cpu )
		  : context( context )
		  , cpu( cpu )
		{ }
	} mmu;

	void SetSR( uint32_t newValue );

public:
	void DumpState();

	virtual void Init();

	virtual void Execute();

	virtual void SetPC( uint64_t pc ) {
		context.pc = pc;
	}

	virtual bool SetSystemReg( int sysReg, uint64_t value ) {
		printf( "Todo:  SetSystemReg( %d, %lx )\n", sysReg, value );
		return false;
	}

	virtual bool ReadSystemReg( int sysReg, uint64_t &/*value*/ ) {
		printf( "Todo:  ReadSystemReg( %d )\n", sysReg );
		return false;
	}

	virtual bool InterpretProcessorSpecific( jit::InterInstr &instr ) {
		printf( "Todo:  InterpretProcessorSpecific( instr.op=%d )\n", instr.op );
		return false;
	}

	Sh4aCpu( jit::Sh4aCpuContext &context )
	  : CpuInterpreter( &context.gpr[0], context.isReserved, context.reservation )
	  , context( context )
	  , mmu( context, *this )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_SH4ACPU_H

