#ifndef GEN7_LIBJITPP_POWERPC_POWERPCINTERMEDIATEBUILDER_H
#define GEN7_LIBJITPP_POWERPC_POWERPCINTERMEDIATEBUILDER_H

#include "jitpp/powerpc/PowerPCHelpers.h"
#include "jitpp/IntermediateBuilder.h"

namespace jitpp {

class PowerPCIntermediateBuilder : public IntermediateBuilder<uint32_t>, private PowerPCHelpers
{
private:
	constexpr int GPR64OFFSET( int regNum ) {
		return regNum * sizeof(uint64_t);
	}

	constexpr int GPR32LOWOFFSET( int regNum ) {
		return regNum * sizeof(uint64_t);
	}

	constexpr int GPR32HIGHOFFSET( int regNum ) {
		return (regNum * sizeof(uint64_t)) + sizeof(uint32_t);
	}

	int BuildIntermediateBcc( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );
	int BuildIntermediateSpecial( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );
	int BuildIntermediateTable19( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );

public:
	virtual int BuildIntermediate( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc );
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_POWERPC_POWERPCINTERMEDIATEBUILDER_H

