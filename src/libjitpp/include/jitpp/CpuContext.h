#ifndef GEN7_LIBJITPP_CPUCONTEXT_H
#define GEN7_LIBJITPP_CPUCONTEXT_H

#include <cstdint>

namespace jitpp {

enum class CpuType : uint32_t {
	UNSPECIFIED = 0,
	XENON       = 1,
	CELL_PPE    = 2,
	CELL_SPE    = 3,
	SH4A        = 4,
};

struct CpuContext {
	const CpuType type;
	const int coreNum;

	CpuContext( CpuType type, int coreNum )
	  : type( type )
	  , coreNum( coreNum )
	{ }
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_CPUCONTEXT_H

