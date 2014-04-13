#ifndef GEN7_LIBJITPP_POWERPC_POWERPCDISASM_H
#define GEN7_LIBJITPP_POWERPC_POWERPCDISASM_H

#include "jitpp/powerpc/PowerPCHelpers.h"
#include "jitpp/Disasm.h"

namespace jitpp {

class PowerPCDisasm : public Disasm, private PowerPCHelpers
{
private:
	void DisassembleBranch( uint32_t instruction, uint32_t pc, char * string );

public:
	virtual void Disassemble( uint8_t *instrBuffer, uint32_t pc, char * string );
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_POWERPC_POWERPCDISASM_H

