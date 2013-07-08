#ifndef GEN7_SHARED_TYPES_H
#define GEN7_SHARED_TYPES_H

#include <cstdint>

namespace Gen7 {

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

struct XenonPpcContext : public CpuContext {
	struct VmxReg {
		uint8_t bytes[ 16 ];
	};

	uint64_t gpr[32];
	uint64_t pc;
	uint64_t lr;
	uint64_t ctr;

	uint64_t fpr[32];
	VmxReg   vmx[128];

	uint64_t msr;
	uint64_t hrmor;

	XenonPpcContext( int coreNum )
	  : CpuContext( CpuType::XENON, coreNum )
	{ }
};

struct Sh4aContext : public CpuContext {
	uint32_t gpr[16];
	uint32_t pc;

	Sh4aContext()
	  : CpuContext( CpuType::SH4A, 0 )
	{ }
};

enum class ConsoleType : int {
	RETAIL,
	DEVKIT,
	ADK,
};

} //namespace Gen7

#endif //GEN7_SHARED_TYPES_H

