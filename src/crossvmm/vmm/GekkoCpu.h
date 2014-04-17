#ifndef GEN7_CROSSVMM_VMM_GEKKOCPU_H
#define GEN7_CROSSVMM_VMM_GEKKOCPU_H

#include "Cpu.h"
#include "ThirtyTwoBitMemoryPolicy.h"

#include "jitpp/powerpc/GekkoCpuContext.h"
#include "jitpp/CpuInterpreter.h"

#include "util/Compilers.h"

class GekkoCpu : public Cpu, public jitpp::CpuInterpreter<ThirtyTwoBitMemoryPolicy<true>>
{
private:
	class PowerPC32Mmu
	{
	private:
		uint64_t *pml3;
		uint64_t *cachedPml2;
		uint64_t *uncachedPml2;

	public:
		void Init();
	};

	PowerPC32Mmu mmu;
	bool isReserved;
	uint64_t reservation;

	jitpp::GekkoCpuContext &context;

	void SetPC( uint64_t pc ) override final {
		context.pc = pc;
	}

	bool SetSystemReg( int sysReg, uint64_t value ) override final {
		UNUSED( sysReg );
		UNUSED( value );

		return false;
	}

	bool ReadSystemReg( int sysReg, uint64_t &value ) override final {
		UNUSED( sysReg );
		UNUSED( value );

		return false;
	}

	bool InterpretProcessorSpecific( jitpp::InterInstr &instr ) override final {
		UNUSED( instr );

		return false;
	}

	template<typename T>
	T* guestVirtToVmmVirt( uint32_t addr ) {
		uint32_t guestPhys = 0;

		switch( addr >> 28 ) {
			case 0x8:
			case 0x9:
				guestPhys = addr - 0x80000000;
				break;

			case 0xA:
			case 0xB:
				guestPhys = addr - 0xA0000000;
				break;
		}

		return reinterpret_cast<T*>( 0xFFFFFFFD00000000UL | guestPhys );
	}

public:
	void Init() override final;
	void Execute() override final;

	GekkoCpu( jitpp::GekkoCpuContext &context )
	  : CpuInterpreter( &context.gpr[0], isReserved, reservation )
	  , context( context )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_GEKKOCPU_H

