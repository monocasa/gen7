#ifndef GEN7_LIBJIT_CPUCONTEXT_H
#define GEN7_LIBJIT_CPUCONTEXT_H

#include <cstdint>

namespace jit {

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

} //namespace jit

#endif //GEN7_LIBJIT_CPUCONTEXT_H

