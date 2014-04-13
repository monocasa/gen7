#ifndef GEN7_LIBJITPP_DISASM_H
#define GEN7_LIBJITPP_DISASM_H

#include <cstdint>

namespace jitpp {

class Disasm
{
public:
	static const int INSTR_BUFFER_SIZE = 16;

	virtual void Disassemble( uint8_t *instrBuffer, uint32_t pc, char * string ) = 0;
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_DISASM_H

