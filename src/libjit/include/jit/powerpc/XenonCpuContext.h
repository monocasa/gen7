#ifndef GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H
#define GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H

#include "jit/CpuContext.h"

#include <cstdint>

namespace jit {

struct VmxReg {
	uint8_t bytes[ 16 ];
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

	bool isReserved;
	uint64_t reservation;

	uint64_t msr;
	uint64_t hrmor;
	uint64_t srr0;
	uint64_t srr1;
	uint64_t hid6;
	uint64_t lpidr;
	uint64_t lpcr;

	XenonCpuContext( int coreNum )
	  : CpuContext( jit::CpuType::XENON, coreNum )
	{ }
};

} //namespace jit

#endif //GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H

