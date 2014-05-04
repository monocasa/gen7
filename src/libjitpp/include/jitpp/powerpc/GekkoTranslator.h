#ifndef GEN7_LIBJITPP_POWERPC_GEKKOTRANSLATOR_H
#define GEN7_LIBJITPP_POWERPC_GEKKOTRANSLATOR_H

#include "jitpp/powerpc/PowerPCDecoder.h"
#include "jitpp/CommonOp.h"

namespace jitpp {

struct CommonOp;

class GekkoTranslator : private PowerPCDecoder
{
private:
	int numOps;
	CommonOp *commonOps;
	uint64_t curPc;

	void PushOp( const CommonOp &op ) {
		commonOps[ numOps++ ] = op;
	}

	void OnUnknownInstruction( uint32_t instr, UnknownCode code, 
	                           int codeArg, uint64_t pc ) override final;

	void OnAddi( int rt, int ra, int16_t si ) override final;
	void OnAddis( int rt, int ra, int16_t si ) override final;
	void OnBranch( uint64_t target, bool link, bool abs ) override final;
	void OnCmpldi( int bf, int ra, uint16_t ui ) override final;
	void OnCmplwi( int bf, int ra, uint16_t ui ) override final;
	void OnLi( int rt, int16_t si ) override final;
	void OnLis( int rt, int16_t si ) override final;
	void OnMflr( int rt ) override final;
	void OnOri( int ra, int rs, uint16_t ui ) override final;

public:
	virtual int BuildOps( CommonOp *commonOps, uint32_t nativeInstr, uint64_t pc );
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_POWERPC_GEKKOTRANSLATOR_H

