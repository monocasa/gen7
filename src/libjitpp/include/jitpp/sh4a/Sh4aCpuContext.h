#ifndef GEN7_LIBJITPP_SH4A_SH4ACPUCONTEXT_H
#define GEN7_LIBJITPP_SH4A_SH4ACPUCONTEXT_H

#include "jitpp/CpuContext.h"

#include <cstdint>

namespace jitpp {

struct Sh4aCpuContext : public CpuContext {
	uint32_t gpr[16];
	uint32_t gpr_bank[8];
	float fpr[16];
	float xpr[16];
	uint32_t sr;
	uint32_t ssr;
	uint32_t gbr;
	uint32_t vbr;
	uint32_t mach;
	uint32_t macl;
	uint32_t pr;
	uint32_t pc;
	uint32_t spc;
	uint32_t fpscr;
	uint32_t fpul;

	uint32_t dbr;

	uint32_t expevt;
	uint32_t qacr0;
	uint32_t qacr1;

	uint32_t sq[ 16 ];

	bool isReserved;
	uint64_t reservation;

	Sh4aCpuContext()
	  : CpuContext( CpuType::SH4A, 0 )
	{ }
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_SH4A_SH4ACPUCONTEXT_H

