#include "jitpp/powerpc/PowerPCDecoder.h"

#include "util/Compilers.h"

namespace jitpp {

void PowerPCDecoder::DecodeSpecial( uint32_t instr, uint64_t pc )
{
	switch( X_XO(instr) ) {
		case SPECIAL_XO_MFSPR: {
			switch( XFX_SPR(instr) ) {
				case SPR_LR: {
					OnMflr( RT(instr) );
				}
				break;

				default: {
					OnUnknownInstruction( instr, UnknownCode::UNKNOWN_SPR_READ, XFX_SPR(instr), pc );
				}
				break;
			}
		}
		return;

		default: {
			OnUnknownInstruction( instr, UnknownCode::SPECIAL, X_XO(instr), pc );
		}
		return;
	}
}

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

		case OPCD_CMPLI: {
			if( D_L(instr) ) {
				OnCmpldi( D_BF(instr), RA(instr), D_UI(instr) );
			}
			else {
				OnCmplwi( D_BF(instr), RA(instr), D_UI(instr) );
			}
		}
		return;

		case OPCD_ORI: {
			OnOri( RA(instr), RS(instr), D_UI(instr) );
		}
		return;

		case OPCD_SPECIAL: {
			DecodeSpecial( instr, pc );
		}
		return;

		default: {
			OnUnknownInstruction( instr, UnknownCode::OPCD, OPCD(instr), pc );
		}
		return;
	}
}

} //namespace jitpp

