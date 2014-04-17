#ifndef GEN7_LIBJITPP_POWERPC_GEKKOCPUCONTEXT_H
#define GEN7_LIBJITPP_POWERPC_GEKKOCPUCONTEXT_H

#include "jitpp/CpuContext.h"

#include <cstdint>

namespace jitpp {

struct GekkoCpuContext : public CpuContext
{
	uint32_t gpr[32];
	uint32_t lr;

	uint32_t pc;

	GekkoCpuContext()
	  : CpuContext( CpuType::GEKKO )
	{ }
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_POWERPC_GEKKOCPUCONTEXT_H

