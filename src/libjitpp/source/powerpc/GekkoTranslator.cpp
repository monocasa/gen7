#include "jitpp/powerpc/GekkoCpuContext.h"
#include "jitpp/powerpc/GekkoTranslator.h"

#include "util/Compilers.h"

namespace jitpp {

void GekkoTranslator::OnUnknownInstruction( uint32_t instr, UnknownCode code, 
                                            int codeArg, uint64_t pc )
{
	UNUSED(instr);
	UNUSED(code);
	UNUSED(codeArg);
	UNUSED(pc);
}

void GekkoTranslator::OnAddi( int rt, int ra, int16_t si )
{
	UNUSED(rt);
	UNUSED(ra);
	UNUSED(si);
}

void GekkoTranslator::OnAddis( int rt, int ra, int16_t si )
{
	UNUSED(rt);
	UNUSED(ra);
	UNUSED(si);
}

void GekkoTranslator::OnBranch( uint64_t target, bool link, bool abs )
{
	UNUSED(abs);

	if( link ) {
		PushOp( CommonOp::BuildLoadRegGpr32ImmU32(GekkoCpuContext::GPR_LR, 
		           curPc + sizeof(uint32_t)) );
	}
	PushOp( CommonOp::BuildBranch(target) );
}

void GekkoTranslator::OnCmpldi( int bf, int ra, uint16_t ui )
{
	UNUSED(bf);
	UNUSED(ra);
	UNUSED(ui);
}

void GekkoTranslator::OnCmplwi( int bf, int ra, uint16_t ui )
{
	UNUSED(bf);
	UNUSED(ra);
	UNUSED(ui);
}

void GekkoTranslator::OnLi( int rt, int16_t si )
{
	UNUSED(rt);
	UNUSED(si);
}

void GekkoTranslator::OnLis( int rt, int16_t si )
{
	UNUSED(rt);
	UNUSED(si);
}

void GekkoTranslator::OnMflr( int rt )
{
	PushOp( CommonOp::BuildLoadRegGpr32Gpr32(rt, GekkoCpuContext::GPR_LR) );
}

void GekkoTranslator::OnOri( int ra, int rs, uint16_t ui )
{
	UNUSED(ra);
	UNUSED(rs);
	UNUSED(ui);
}

void GekkoTranslator::OnOris( int ra, int rs, uint16_t ui )
{
	PushOp( CommonOp::BuildOrImm32(ra, rs, ui << 16) );
}

int GekkoTranslator::BuildOps( CommonOp *commonOps, uint32_t nativeInstr, uint64_t pc )
{
	numOps = 0;
	this->commonOps = commonOps;
	curPc = pc;

	DecodeInstruction( nativeInstr, pc );

	return numOps;
}

} //namespace jitpp

