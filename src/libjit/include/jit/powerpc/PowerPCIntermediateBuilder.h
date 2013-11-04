#ifndef GEN7_LIBJIT_POWERPC_POWERPCINTERMEDIATEBUILDER_H
#define GEN7_LIBJIT_POWERPC_POWERPCINTERMEDIATEBUILDER_H

#include "jit/powerpc/PowerPCHelpers.h"
#include "jit/IntermediateBuilder.h"

namespace jit {

class PowerPCIntermediateBuilder : public IntermediateBuilder<uint32_t>, private PowerPCHelpers
{
private:
	constexpr int GPR64OFFSET( int regNum ) {
		return regNum * sizeof(uint64_t);
	}

	constexpr int GPR32LOWOFFSET( int regNum ) {
		return regNum * sizeof(uint64_t);
	}

public:
	virtual int BuildIntermediate( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );
};

}

#endif //GEN7_LIBJIT_POWERPC_POWERPCINTERMEDIATEBUILDER_H

