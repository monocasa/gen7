#ifndef GEN7_CROSSVMM_VMM_PPCCPU_H
#define GEN7_CROSSVMM_VMM_PPCCPU_H

#include "Cpu.h"
#include "ThirtyTwoBitMemoryPolicy.h"

#include "jitpp/powerpc/PowerPCIntermediateBuilder.h"
#include "jitpp/powerpc/XenonCpuContext.h"
#include "jitpp/CpuInterpreter.h"
#include "jitpp/InterInstr.h"

#include <cstdio>
#include <ucontext.h>

class PpcCpu : public Cpu, 
               public jitpp::CpuInterpreter<ThirtyTwoBitMemoryPolicy<true>>, 
               private jitpp::PowerPCIntermediateBuilder
{
private:
	jitpp::XenonCpuContext &context;

	mcontext_t error_context;

	volatile bool readingInstructions;
	uint64_t faultingAddr;

	class MmuContext
	{
	private:
		struct TlbEntry {
			uint64_t vpn;
			uint64_t rpn;

			bool IsValid() {
				return (vpn & 1) != 0;
			}

			uint64_t GetVpn() {
				return (vpn << 16) & 0xFF000000;
			}

			uint64_t GetRpn() {
				return rpn & 0xFFFFFFFFFF000000UL;
			}
		};

		static const int NUM_TLB_ENTRIES = 1024;

		static const int NUM_HVREALMODE_PML3S = 4;

		static const int NUM_IR32_PML2S = 4;
		static const int NUM_IR32_PML1S = 1024;

		jitpp::XenonCpuContext &context;

		uint64_t* hvRealModePml3s[ NUM_HVREALMODE_PML3S ];  //PML3s for Hypervisor Real Mode

		uint64_t* ir32Pml3;                     //PML3 for instruction relocate/32 bit
		uint64_t* ir32Pml2s[ NUM_IR32_PML2S ]; //PML2s for instruction relocate/32 bit
		uint64_t* ir32Pml1s[ NUM_IR32_PML1S ]; //PML1s for instruction relocate/32 bit

		int nextTlbHint;

		int currentTlbIndex;
		int currentTlbSets;

		TlbEntry tlbEntries[ 256 ][ 4 ];

		void SetNewVpn( uint64_t value, int tlbIndex, int tlbSet );
		void Map16MPage( uint64_t vpn, uint64_t rpn );

	public:
		void Init();

		void MapFull();

		void DisableRelocation();
		void EnableRelocation();

		bool IsInstructionMapped( uint64_t addr );

		int GetTlbHint();

		void WriteTlbIndex( uint64_t value ) {
			currentTlbIndex = ((value >> 4) & 0xFF);
			currentTlbSets = value & 0xf;
		}

		void WriteTlbVpn( uint64_t value );

		void WriteTlbRpn( uint64_t value );

		MmuContext( jitpp::XenonCpuContext &context )
		  : context( context )
		  , nextTlbHint( 0x3FF )
		{ }

	} mmuContext;

	class InterpreterPageFaultHandler : public PageFaultHandler
	{
	private:
		PpcCpu &cpu;

	public:
		virtual void OnPageFault( PageFaultReason reason, uint64_t addr, InterruptRegs *regs ) {
			cpu.OnInterpPageFault( reason, addr, regs );
		}

		InterpreterPageFaultHandler( PpcCpu &cpu )
		  : cpu( cpu )
		{ }
	} interpPageFaultHandler;

	void DumpContext();

	void SetMsr( uint64_t newMsr );

	void OnInterpPageFault( PageFaultReason reason, uint64_t addr, InterruptRegs *regs );

public:
	virtual void Init();

	virtual void Execute();

	virtual void SetPC( uint64_t pc );

	virtual bool SetSystemReg( int sysReg, uint64_t value );
	virtual bool ReadSystemReg( int sysReg, uint64_t &value );

	virtual bool InterpretProcessorSpecific( jitpp::InterInstr &instr );

	PpcCpu( jitpp::XenonCpuContext &context )
	  : CpuInterpreter( &context.gpr[0], context.isReserved, context.reservation )
	  , context( context )
	  , mmuContext( context )
	  , interpPageFaultHandler( *this )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_PPCCPU_H

