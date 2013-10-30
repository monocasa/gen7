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
	uint64_t lr;
	uint64_t ctr;
	uint64_t sprg0;
	uint64_t sprg1;
	uint64_t sprg2;
	uint64_t sprg3;

	uint64_t fpr[32];
	VmxReg   vmx[128];

	uint64_t pc;

	uint64_t msr;
	uint64_t hrmor;
	uint64_t srr0;
	uint64_t srr1;
	uint64_t hid6;
	uint64_t lpidr;
	uint64_t lpcr;

	XenonPpcContext( int coreNum )
	  : CpuContext( CpuType::XENON, coreNum )
	{ }
};

struct Sh4aContext : public CpuContext {
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

