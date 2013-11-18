#ifndef GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H
#define GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H

#include "jit/CpuContext.h"

#include <cstdint>

namespace jit {

struct VmxReg {
	uint8_t bytes[ 16 ];
};

struct ConditionRegister {
	union {
		struct {
			uint8_t so : 1;
			uint8_t eq : 1;
			uint8_t gt : 1;
			uint8_t lt : 1;
		};
		uint8_t nybble : 4;
	};
};

struct XenonCpuContext : public CpuContext {
	uint64_t gpr[32];
	uint64_t lr;
	uint64_t ctr;
	uint64_t sprg0;
	uint64_t sprg1;
	uint64_t sprg2;
	uint64_t sprg3;

	uint64_t fpr[32];
	VmxReg   vmx[128];

	uint64_t pc;

	ConditionRegister cr[8];

	bool isReserved;
	uint64_t reservation;

	uint64_t msr;
	uint64_t hrmor;
	uint64_t srr0;
	uint64_t srr1;
	uint64_t hid6;
	uint64_t lpidr;
	uint64_t lpcr;

	uint32_t ReadCr() const {
		uint32_t ret = 0;
		for( int i = 0; i < 8; i++ ) {
			ret <<= 4;
			ret |= cr[i].nybble & 0xF;
		}
		return ret;
	}

	XenonCpuContext( int coreNum )
	  : CpuContext( jit::CpuType::XENON, coreNum )
	{ }
};

} //namespace jit

#endif //GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H

