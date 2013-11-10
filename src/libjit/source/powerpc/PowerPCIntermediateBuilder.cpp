#include "jit/powerpc/PowerPCIntermediateBuilder.h"
#include "jit/InterInstr.h"

namespace jit {

int PowerPCIntermediateBuilder::BuildIntermediate( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc )
{
	const int opcd = OPCD(nativeInstr);

	switch( opcd ) {
		case OPCD_ADDI: {
			const int64_t imm = D_SI(nativeInstr);
			const int rt = D_RT(nativeInstr);
			const int ra = D_RA(nativeInstr);

			if( ra != 0 ) {
				intermediates[0].BuildAddImm( GPR64OFFSET(ra), GPR64OFFSET(rt), imm );
				return 1;
			}
			else {
				intermediates[0].BuildLoadImm( GPR64OFFSET(rt), imm );
				return 1;
			}
		}			

		case OPCD_ADDIS: {
			const int64_t imm = D_SI(nativeInstr) << 16;
			const int rt = D_RT(nativeInstr);
			const int ra = D_RA(nativeInstr);

			if( ra != 0 ) {
				intermediates[0].BuildAddImm( GPR64OFFSET(ra), GPR64OFFSET(rt), imm );
				return 1;
			}
			else {
				intermediates[0].BuildLoadImm( GPR64OFFSET(rt), imm );
				return 1;
			}
		}

		case OPCD_BRANCH: {
			uint64_t target = B_LI(nativeInstr);

			if( !B_AA(nativeInstr) ) {
				target += pc;
			}

			if( B_LK(nativeInstr) ) {
				intermediates[0].BuildLoadImm( 32 * sizeof(uint64_t), pc + sizeof(uint32_t) );
				intermediates[1].BuildBranchAlways( target );
				return 2;
			}
			else {
				intermediates[0].BuildBranchAlways( target );
				return 1;
			}
		}

		default: {
			intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
			return 1;
		}
	}
}

} //namespace jit

