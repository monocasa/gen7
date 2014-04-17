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
	GEKKO       = 5,
};

struct CpuContext {
	const CpuType type;
	const int coreNum;

	CpuContext( CpuType type, int coreNum )
	  : type( type )
	  , coreNum( coreNum )
	{ }

	CpuContext( CpuType type )
	  : type( type )
	  , coreNum( 0 )
	{ }
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_CPUCONTEXT_H

