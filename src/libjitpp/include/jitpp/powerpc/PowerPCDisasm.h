#ifndef GEN7_LIBJITPP_POWERPC_POWERPCDISASM_H
#define GEN7_LIBJITPP_POWERPC_POWERPCDISASM_H

#include "jitpp/powerpc/PowerPCDecoder.h"
#include "jitpp/Disasm.h"

namespace jitpp {

class PowerPCDisasm : public Disasm, private PowerPCDecoder
{
private:
	char *buffer;

	void OnUnknownInstruction( uint32_t instr, UnknownCode code, 
	                           int codeArg, uint64_t pc ) override final;

	void OnBranch( uint64_t target, bool link, bool abs ) override final;

public:
	virtual void Disassemble( uint8_t *instrBuffer, uint32_t pc, char * string );
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_POWERPC_POWERPCDISASM_H

