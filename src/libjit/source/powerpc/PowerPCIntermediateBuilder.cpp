#include "jit/powerpc/PowerPCIntermediateBuilder.h"
#include "jit/InterInstr.h"

namespace jit {

int PowerPCIntermediateBuilder::BuildIntermediateBcc( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc )
{
	if( B_LK(nativeInstr) || B_AA(nativeInstr) ) {
		intermediates[0].BuildUnknown( 16, nativeInstr, pc );
		return 1;
	}

	if( B_BI(nativeInstr) ) {
		intermediates[0].BuildUnknown( 16, nativeInstr, pc );
		return 1;
	}

	const uint64_t target = pc + B_BD( nativeInstr );

	switch( B_BO(nativeInstr) ) {
		case 25: { //bdnz+
			intermediates[0].BuildSubuImm( GPR64OFFSET(GPR_CTR), GPR64OFFSET(GPR_CTR), 1 );
			intermediates[1].BuildBranchGprNotZero( GPR64OFFSET(GPR_CTR), target );
			return 2;
		}

		default: {
			intermediates[0].BuildUnknown( 16, nativeInstr, pc );
			return 1;
		}
	}
}

int PowerPCIntermediateBuilder::BuildIntermediateSpecial( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc )
{
	const int xo = X_XO(nativeInstr);

	switch( xo ) {
		case SPECIAL_XO_ADD: {
			if( X_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}
			const int rs = GPR64OFFSET( X_RS(nativeInstr) );
			const int ra = GPR64OFFSET( X_RA(nativeInstr) );
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );

			intermediates[0].BuildAdd( rs, rb, ra );
			return 1;
		}

		case SPECIAL_XO_ANDC: {
			if( X_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}
			const int rs = GPR64OFFSET( X_RS(nativeInstr) );
			const int ra = GPR64OFFSET( X_RA(nativeInstr) );
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );

			intermediates[0].BuildAndc( rs, rb, ra );
			return 1;
		}

		case SPECIAL_XO_EIEIO: {
			intermediates[0].BuildNop();
			return 1;
		}

		case SPECIAL_XO_MFMSR: {
			const int rt = GPR64OFFSET( X_RT(nativeInstr) );

			intermediates[0].BuildReadSystem( rt, SPR_MSR );
			return 1;
		}

		case SPECIAL_XO_MFSPR: {
			const int spr = XFX_SPR(nativeInstr);
			const int destReg = GPR64OFFSET( XFX_RS(nativeInstr) );

			switch( spr ) {
				case SPR_LR: {
					intermediates[0].BuildMoveReg( GPR64OFFSET(GPR_LR), destReg );
					break;
				}
				case SPR_CTR: {
					intermediates[0].BuildMoveReg( GPR64OFFSET(GPR_CTR), destReg );
					break;
				}
				case SPR_SPRG0: {
					intermediates[0].BuildMoveReg( GPR64OFFSET(GPR_SPRG0), destReg );
					break;
				}
				case SPR_SPRG1: {
					intermediates[0].BuildMoveReg( GPR64OFFSET(GPR_SPRG1), destReg );
					break;
				}
				case SPR_SPRG2: {
					intermediates[0].BuildMoveReg( GPR64OFFSET(GPR_SPRG2), destReg );
					break;
				}
				case SPR_SPRG3: {
					intermediates[0].BuildMoveReg( GPR64OFFSET(GPR_SPRG3), destReg );
					break;
				}
				default: {
					intermediates[0].BuildReadSystem( destReg, spr );
					break;
				}
			}

			return 1;
		}

		case SPECIAL_XO_MTSPR: {
			const int spr = XFX_SPR(nativeInstr);
			const int sourceReg = GPR64OFFSET( XFX_RS(nativeInstr) );

			switch( spr ) {
				case SPR_LR: {
					intermediates[0].BuildMoveReg( sourceReg, GPR64OFFSET(GPR_LR) );
					break;
				}
				case SPR_CTR: {
					intermediates[0].BuildMoveReg( sourceReg, GPR64OFFSET(GPR_CTR) );
					break;
				}
				case SPR_SPRG0: {
					intermediates[0].BuildMoveReg( sourceReg, GPR64OFFSET(GPR_SPRG0) );
					break;
				}
				case SPR_SPRG1: {
					intermediates[0].BuildMoveReg( sourceReg, GPR64OFFSET(GPR_SPRG1) );
					break;
				}
				case SPR_SPRG2: {
					intermediates[0].BuildMoveReg( sourceReg, GPR64OFFSET(GPR_SPRG2) );
					break;
				}
				case SPR_SPRG3: {
					intermediates[0].BuildMoveReg( sourceReg, GPR64OFFSET(GPR_SPRG3) );
					break;
				}
				default: {
					intermediates[0].BuildSetSystemReg( sourceReg, spr );
					break;
				}
			}

			return 1;
		}

		case SPECIAL_XO_OR: {
			if( X_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}
			const int rs = GPR64OFFSET( X_RS(nativeInstr) );
			const int ra = GPR64OFFSET( X_RA(nativeInstr) );
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );

			intermediates[0].BuildOr( rs, rb, ra );
			return 1;
		}

		case SPECIAL_XO_SLBIA: {
			intermediates[0].BuildPpcSlbia();
			return 1;
		}

		case SPECIAL_XO_SLBMTE: {
			const int rs = GPR64OFFSET( X_RS(nativeInstr) );
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );

			intermediates[0].BuildPpcSlbmte( rs, rb );
			return 1;
		}

		case SPECIAL_XO_SUBF: {
			if( XO_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}

			const int rt = GPR64OFFSET( XO_RT(nativeInstr) );
			const int ra = GPR64OFFSET( XO_RA(nativeInstr) );
			const int rb = GPR64OFFSET( XO_RB(nativeInstr) );

			intermediates[0].BuildSub( ra, rb, rt );

			return 1;
		}

		case SPECIAL_XO_SYNC: {
			intermediates[0].BuildNop();
			return 1;
		}

		case SPECIAL_XO_TLBIEL: {
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );
			const bool l = X_L(nativeInstr);

			intermediates[0].BuildPpcTlbiel( rb, l );
			return 1;
		}

		default: {
			intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
			return 1;
		}
	}
}

int PowerPCIntermediateBuilder::BuildIntermediateTable19( InterInstr *intermediates, uint32_t nativeInstr, uint64_t pc )
{
	const int xo = X_XO(nativeInstr);

	switch( xo ) {
		case TABLE_19_XO_ISYNC: {
			intermediates[0].BuildNop();
			return 1;
		}

		default: {
			intermediates[0].BuildUnknown( xo + 1900000, nativeInstr, pc );
			return 1;
		}
	}
}

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

		case OPCD_BCC: {
			return BuildIntermediateBcc( intermediates, nativeInstr, pc );
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

		case OPCD_ROTATE_21: {
			const int rs = GPR32LOWOFFSET( M_RS(nativeInstr) );
			const int ra = GPR32LOWOFFSET( M_RA(nativeInstr) );

			if( (nativeInstr & 0xFFFF) == 0x901A ) { //slwi rs, ra, 18
				intermediates[0].BuildSll32Imm( rs, ra, 18 );
			}
			else {
				intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
			}

			return 1;
		}

		case OPCD_ROTATE_30: {
			const int rs = GPR64OFFSET( D_RS(nativeInstr) );
			const int ra = GPR64OFFSET( D_RA(nativeInstr) );

			if( (nativeInstr & 0xFFFF) == 0x07C6 ) { //sldi rs, ra, 32
				intermediates[0].BuildSll64Imm( rs, ra, 32 );
			}
			else if( (nativeInstr & 0xFFFF) == 0x64c6 ) { //sldi rs, ra, 44
				intermediates[0].BuildSll64Imm( rs, ra, 44 );
			}
			else if( (nativeInstr & 0xFFFF) == 0xf806 ) { //sldi rs, ra, 63
				intermediates[0].BuildSll64Imm( rs, ra, 63 );
			}
			else if( (nativeInstr & 0xFFFF) == 0x0040 ) { //clrldi rs, ra, 1
				intermediates[0].BuildAndImm( rs, ra, 0x7FFFFFFFFFFFFFFFUL );
			}
			else {
				intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
			}
			return 1;
		}

		case OPCD_ORI: {
			const int rs = GPR64OFFSET( D_RS(nativeInstr) );
			const int ra = GPR64OFFSET( D_RA(nativeInstr) );
			const uint64_t imm = D_UI(nativeInstr);

			if( rs == 0 && ra == 0 && imm == 0 ) {
				intermediates[0].BuildNop();
				return 1;
			}

			intermediates[0].BuildOrImm( rs, ra, imm );
			return 1;
		}

		case OPCD_ORIS: {
			const int rs = GPR64OFFSET( D_RS(nativeInstr) );
			const int ra = GPR64OFFSET( D_RA(nativeInstr) );
			const uint64_t imm = D_UI(nativeInstr) << 16;

			if( rs == 0 && ra == 0 && imm == 0 ) {
				intermediates[0].BuildNop();
				return 1;
			}

			intermediates[0].BuildOrImm( rs, ra, imm );
			return 1;
		}

		case OPCD_SPECIAL: {
			return BuildIntermediateSpecial( intermediates, nativeInstr, pc );
		}

		case OPCD_TABLE_19: {
			return BuildIntermediateTable19( intermediates, nativeInstr, pc );
		}

		default: {
			intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
			return 1;
		}
	}
}

} //namespace jit

