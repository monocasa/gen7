#ifndef GEN7_CROSSVMM_VMM_GEKKOCPU_H
#define GEN7_CROSSVMM_VMM_GEKKOCPU_H

#include "Cpu.h"
#include "ThirtyTwoBitMemoryPolicy.h"

#include "jitpp/powerpc/GekkoCpuContext.h"
#include "jitpp/CommonOpInterpreter.h"

#include "util/Compilers.h"

class GekkoCpu : public Cpu, 
                 private jitpp::CommonOpInterpreter<ThirtyTwoBitMemoryPolicy<true>>
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

	void SetPc( uint64_t pc ) override final {
		context.pc = pc;
	}

	template<typename T>
	T* guestVirtToVmmVirt( uint32_t addr ) {
		uint32_t guestPhys = 0;

		switch( addr >> 28 ) {
			case 0x8:
			case 0x9:
			case 0xA:
			case 0xB:
				guestPhys = addr - 0x80000000;
				break;

			case 0xC:
			case 0xD:
			case 0xE:
			case 0xF:
				guestPhys = addr - 0xC0000000;
				break;
		}

		return reinterpret_cast<T*>( 0xFFFFFFFD00000000UL | guestPhys );
	}

	void DumpState();
	void DumpPosition();

public:
	void Init() override final;
	void Execute() override final;

	GekkoCpu( jitpp::GekkoCpuContext &context )
	  : CommonOpInterpreter( &context.gpr[0] )
	  , context( context )
	{ }
};

#endif //GEN7_CROSSVMM_VMM_GEKKOCPU_H

