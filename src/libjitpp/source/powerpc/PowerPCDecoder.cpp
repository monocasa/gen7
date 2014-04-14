#include "jitpp/powerpc/PowerPCDecoder.h"

#include "util/Compilers.h"

namespace jitpp {

void PowerPCDecoder::DecodeInstruction( uint32_t instr, uint64_t pc )
{
	switch( OPCD(instr) ) {
		case OPCD_BRANCH: {
			const bool link = B_LK( instr );
			const bool abs = B_AA( instr );

			const uint64_t target = B_LI( instr ) + ((abs) ? 0 : pc );

			OnBranch( target, link, abs );
		}
		return;

		default: {
			OnUnknownInstruction( instr, UnknownCode::OPCD, OPCD(instr), pc );
		}
		return;
	}
}

} //namespace jitpp

