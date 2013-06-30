#ifndef GEN7_SHARED_TYPES_H
#define GEN7_SHARED_TYPES_H

#include <cstdint>

namespace Gen7 {

struct XenonPpcContext {
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

	int coreNum;
};

enum class ConsoleType : int {
	RETAIL,
	DEVKIT,
	ADK,
};

} //namespace Gen7

#endif //GEN7_SHARED_TYPES_H

