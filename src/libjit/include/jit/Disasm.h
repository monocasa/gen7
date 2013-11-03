#ifndef GEN7_LIBJIT_DISASM_H
#define GEN7_LIBJIT_DISASM_H

#include <cstdint>

namespace jit {

class Disasm
{
public:
	static const int INSTR_BUFFER_SIZE = 16;

	virtual void Disassemble( uint8_t *instrBuffer, uint32_t pc, char * string ) = 0;
};

} //namespace jit

#endif //GEN7_LIBJIT_DISASM_H

