#include "jitpp/powerpc/PowerPCDecoder.h"

#include "util/Compilers.h"

namespace jitpp {

void PowerPCDecoder::DecodeInstruction( uint32_t instr, uint64_t pc )
{
	switch( OPCD(instr) ) {
		case OPCD_ADDI: {
			if( RA(instr) != 0 ) {
				OnAddi( RT(instr), RA(instr), D_SI(instr) );
			}
			else {
				OnLi( RT(instr), D_SI(instr) );
			}
		}
		return;

		case OPCD_ADDIS: {
			if( RA(instr) != 0 ) {
				OnAddis( RT(instr), RA(instr), D_SI(instr) );
			}
			else {
				OnLis( RT(instr), D_SI(instr) );
			}
		}
		return;

		case OPCD_BRANCH: {
			const bool link = B_LK( instr );
			const bool abs = B_AA( instr );

			const uint64_t target = B_LI( instr ) + ((abs) ? 0 : pc );

			OnBranch( target, link, abs );
		}
		return;

		case OPCD_ORI: {
			OnOri( RA(instr), RS(instr), D_UI(instr) );
		}
		return;

		default: {
			OnUnknownInstruction( instr, UnknownCode::OPCD, OPCD(instr), pc );
		}
		return;
	}
}

} //namespace jitpp

