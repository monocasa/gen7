#ifndef GEN7_LIBJIT_POWERPC_POWERPCDISASM_H
#define GEN7_LIBJIT_POWERPC_POWERPCDISASM_H

#include "jit/powerpc/PowerPCHelpers.h"
#include "jit/Disasm.h"

namespace jit {

class PowerPCDisasm : public Disasm, private PowerPCHelpers
{
private:
	void DisassembleBranch( uint32_t instruction, uint32_t pc, char * string );

public:
	virtual void Disassemble( uint8_t *instrBuffer, uint32_t pc, char * string );
};

} //namespace jit

#endif //GEN7_LIBJIT_POWERPC_POWERPCDISASM_H

