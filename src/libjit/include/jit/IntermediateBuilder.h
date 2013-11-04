#ifndef GEN7_LIBJIT_INTERMEDIATEBUILDER_H
#define GEN7_LIBJIT_INTERMEDIATEBUILDER_H

#include <cstdint>

namespace jit {

struct InterInstr;

template<typename INSTR_FORMAT>
class IntermediateBuilder
{
public:
	virtual int BuildIntermediate( InterInstr *intermediates, INSTR_FORMAT nativeInstr, uint64_t pc ) = 0;
};

}

#endif //GEN7_LIBJIT_INTERMEDIATEBUILDER_H

