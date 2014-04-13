#ifndef GEN7_LIBJITPP_INTERMEDIATEBUILDER_H
#define GEN7_LIBJITPP_INTERMEDIATEBUILDER_H

#include <cstdint>

namespace jitpp {

struct InterInstr;

template<typename INSTR_FORMAT>
class IntermediateBuilder
{
public:
	virtual int BuildIntermediate( InterInstr *intermediates, INSTR_FORMAT nativeInstr, uint64_t pc ) = 0;
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_INTERMEDIATEBUILDER_H

