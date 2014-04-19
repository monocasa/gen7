#include "jitpp/powerpc/PowerPCIntermediateBuilder.h"
#include "jitpp/InterInstr.h"

#include <cstdio>

namespace jitpp {

int PowerPCIntermediateBuilder::BuildIntermediateBcc( InterInstr *intermediates, uint32_t nativeInstr, const uint64_t pc )
{
	if( B_AA(nativeInstr) ) {
		intermediates[0].BuildUnknown( 1600000 + 1025, nativeInstr, pc );
		return 1;
	}

	const uint64_t target = pc + B_BD( nativeInstr );

	int instrCount = 0;

	switch( B_BO(nativeInstr) ) {
		case 4:   //bcc cr clear
		case 6:   //bcc cr clear-
		case 7: { //bcc cr clear+
			intermediates[instrCount++].BuildBranchGpr32MaskZero( GPR32LOWOFFSET(GPR_CR), 0x80000000 >> B_BI(nativeInstr), target );
			break;
		}

		case 12:   //bcc cr set
		case 15: { //bcc cr set +
			intermediates[instrCount++].BuildBranchGpr32MaskNotZero( GPR32LOWOFFSET(GPR_CR), 0x80000000 >> B_BI(nativeInstr), target );
			break;
		}

		case 20: { //branch always
			intermediates[instrCount++].BuildBranchAlways( target );
			break;
		}

		case 25: { //bdnz+
			intermediates[instrCount++].BuildSubuImm( GPR64OFFSET(GPR_CTR), GPR64OFFSET(GPR_CTR), 1 );
			intermediates[instrCount++].BuildBranchGpr64NotZero( GPR64OFFSET(GPR_CTR), target );
			break;
		}

		default: {
			intermediates[0].BuildUnknown( 1600000 + B_BO(nativeInstr), nativeInstr, pc );
			return 1;
		}
	}

	if( B_LK(nativeInstr) ) {
		intermediates[instrCount++].BuildLoad64Imm( GPR64OFFSET(GPR_LR), pc + sizeof(uint32_t) );
	}

	return instrCount;
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
			const int rt = GPR64OFFSET( X_RT(nativeInstr) );
			const int ra = GPR64OFFSET( X_RA(nativeInstr) );
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );

			intermediates[0].BuildAdd( rt, ra, rb );
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

		case SPECIAL_XO_CMP: {
			const int ra = X_RA( nativeInstr );
			const int rb = X_RB( nativeInstr );

			const int cr = X_BF( nativeInstr );

			if( X_L(nativeInstr) ) {
				intermediates[0].BuildPpcCmpd( GPR64OFFSET(ra), GPR64OFFSET(rb), cr );
			}
			else {
				intermediates[0].BuildPpcCmpw( GPR32LOWOFFSET(ra), GPR32LOWOFFSET(rb), cr );
			}

			return 1;
		}

		case SPECIAL_XO_CMPL: {
			const int ra = X_RA( nativeInstr );
			const int rb = X_RB( nativeInstr );

			const int cr = X_BF( nativeInstr );

			if( X_L(nativeInstr) ) {
				intermediates[0].BuildPpcCmpld( GPR64OFFSET(ra), GPR64OFFSET(rb), cr );
			}
			else {
				intermediates[0].BuildPpcCmplw( GPR32LOWOFFSET(ra), GPR32LOWOFFSET(rb), cr );
			}

			return 1;
		}

		case SPECIAL_XO_EIEIO: {
			intermediates[0].BuildNop();
			return 1;
		}

		case SPECIAL_XO_LWARX: {
			const int rt = X_RT(nativeInstr);
			const int ra = X_RA(nativeInstr);
			const int rb = X_RB(nativeInstr);

			int instrCount = 0;

			if( 0 == ra ) {
				intermediates[instrCount++].BuildLoad32Linked( GPR32LOWOFFSET(rt),
				                                               GPR64OFFSET(rb) );
			}
			else {
				intermediates[instrCount++].BuildAdd( GPR64OFFSET(GPR_TEMP),
				                                      GPR64OFFSET(ra),
				                                      GPR64OFFSET(rb) );
				intermediates[instrCount++].BuildLoad32Linked( GPR32LOWOFFSET(rt),
				                                               GPR64OFFSET(GPR_TEMP) );
			}

			intermediates[instrCount++].BuildLoad32Imm( GPR32HIGHOFFSET(rt), 0 );

			return instrCount;
		}

		case SPECIAL_XO_LWBRX: {
			const int rt = X_RT(nativeInstr);
			const int ra = X_RA(nativeInstr);
			const int rb = X_RB(nativeInstr);

			if( ra == 0 ) {
				intermediates[0].BuildLoad32RegOffset( GPR32LOWOFFSET(rt), GPR64OFFSET(rb), 0 );
				intermediates[1].BuildByteSwap32( GPR32LOWOFFSET(rt), GPR32LOWOFFSET(rt) );
				intermediates[2].BuildLoad32Imm( GPR32HIGHOFFSET(rt), 0 );
				return 3;
			}
			else {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}
		}

		case SPECIAL_XO_MFCR: {
			const int rt = XFX_RT(nativeInstr);

			if( (nativeInstr >> 20) & 1 ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}

			intermediates[0].BuildMoveReg32( GPR32LOWOFFSET(rt), GPR32LOWOFFSET(GPR_CR) );
			intermediates[1].BuildLoad32Imm( GPR32HIGHOFFSET(rt), 0 );

			return 2;
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
					intermediates[0].BuildMoveReg64( destReg, GPR64OFFSET(GPR_LR) );
					break;
				}
				case SPR_CTR: {
					intermediates[0].BuildMoveReg64( destReg, GPR64OFFSET(GPR_CTR) );
					break;
				}
				case SPR_SPRG0: {
					intermediates[0].BuildMoveReg64( destReg, GPR64OFFSET(GPR_CTR) );
					break;
				}
				case SPR_SPRG1: {
					intermediates[0].BuildMoveReg64( destReg, GPR64OFFSET(GPR_SPRG1) );
					break;
				}
				case SPR_SPRG2: {
					intermediates[0].BuildMoveReg64( destReg, GPR64OFFSET(GPR_SPRG2) );
					break;
				}
				case SPR_SPRG3: {
					intermediates[0].BuildMoveReg64( destReg, GPR64OFFSET(GPR_SPRG2) );
					break;
				}
				default: {
					intermediates[0].BuildReadSystem( destReg, spr );
					break;
				}
			}

			return 1;
		}

		case SPECIAL_XO_MTCRF: {
			if( XFX_FXM(nativeInstr) != 0xFF ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}

			const int rs = XFX_RS(nativeInstr);

			intermediates[0].BuildMoveReg32( GPR32LOWOFFSET(GPR_CR), GPR32LOWOFFSET(rs) );
			return 1;
		}

		case SPECIAL_XO_MTSPR: {
			const int spr = XFX_SPR(nativeInstr);
			const int sourceReg = GPR64OFFSET( XFX_RS(nativeInstr) );

			switch( spr ) {
				case SPR_LR: {
					intermediates[0].BuildMoveReg64( GPR64OFFSET(GPR_LR), sourceReg );
					break;
				}
				case SPR_CTR: {
					intermediates[0].BuildMoveReg64( GPR64OFFSET(GPR_CTR), sourceReg );
					break;
				}
				case SPR_SPRG0: {
					intermediates[0].BuildMoveReg64( GPR64OFFSET(GPR_SPRG0), sourceReg );
					break;
				}
				case SPR_SPRG1: {
					intermediates[0].BuildMoveReg64( GPR64OFFSET(GPR_SPRG1), sourceReg );
					break;
				}
				case SPR_SPRG2: {
					intermediates[0].BuildMoveReg64( GPR64OFFSET(GPR_SPRG2), sourceReg );
					break;
				}
				case SPR_SPRG3: {
					intermediates[0].BuildMoveReg64( GPR64OFFSET(GPR_SPRG3), sourceReg );
					break;
				}
				default: {
					intermediates[0].BuildSetSystemReg( spr, sourceReg );
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

		case SPECIAL_XO_SLW: {
			if( X_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( xo + 3100000, nativeInstr, pc );
				return 1;
			}

			const int rs = GPR64OFFSET( X_RS(nativeInstr) );
			const int ra = GPR64OFFSET( X_RA(nativeInstr) );
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );

			intermediates[0].BuildSll32( rs, ra, rb );

			return 1;
		}

		case SPECIAL_XO_SRAWI: {
			const int rs = GPR64OFFSET( X_RS(nativeInstr) );
			const int ra = GPR64OFFSET( X_RA(nativeInstr) );
			const int sh = X_SH(nativeInstr);
			const bool rc = X_RC(nativeInstr);

			intermediates[0].BuildPpcSrawi( rs, ra, sh, rc );

			return 1;
		}

		case SPECIAL_XO_STWCX: {
			const int rt = GPR32LOWOFFSET( X_RT(nativeInstr) );
			const int ra = GPR64OFFSET( X_RA(nativeInstr) );
			const int rb = GPR64OFFSET( X_RB(nativeInstr) );

			intermediates[0].BuildPpcStwcx( rb, ra, rt );

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
		case TABLE_19_XO_BCLR: {
			if( nativeInstr == 0x4e800020 ) { //blr
				intermediates[0].BuildBranchGpr64( GPR64OFFSET(GPR_LR) );
				return 1;
			}
			else if( nativeInstr == 0x4d9e0020 ) { //beqlr    cr7
				intermediates[0].BuildAndImm64( GPR64OFFSET(GPR_CR), GPR64OFFSET(GPR_TEMP), 0x00000002 );
				intermediates[1].BuildBranchGpr64NotZeroGpr64( GPR64OFFSET(GPR_TEMP), GPR64OFFSET(GPR_LR) );
				return 2;
			}
			else {
				intermediates[0].BuildUnknown( xo + 1900000, nativeInstr, pc );
				return 1;
			}
		}

		case TABLE_19_XO_BCCTR: {
			if( nativeInstr == 0x4e800421 ) { //bctrl
				intermediates[0].BuildBranchGpr64( GPR64OFFSET(GPR_CTR) );
				intermediates[1].BuildLoad64Imm( GPR64OFFSET(GPR_LR), pc + sizeof(uint32_t) );
				return 2;
			}
			else {
				intermediates[0].BuildUnknown( xo + 1900000, nativeInstr, pc );
				return 1;
			}
		}

		case TABLE_19_XO_CRXOR: {
			if( nativeInstr == 0x4cc63182 ) {
				intermediates[0].BuildAndImm32( GPR32LOWOFFSET(GPR_CR), GPR32LOWOFFSET(GPR_CR), 0xFDFFFFFF );
				return 1;
			}
			else {
				intermediates[0].BuildUnknown( xo + 1900000, nativeInstr, pc );
				return 1;
			}
		}

		case TABLE_19_XO_ISYNC: {
			intermediates[0].BuildNop();
			return 1;
		}

		case TABLE_19_XO_RFID: {
			intermediates[0].BuildPpcRfid();
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
				intermediates[0].BuildLoad64Imm( GPR64OFFSET(rt), imm );
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
				intermediates[0].BuildLoad64Imm( GPR64OFFSET(rt), imm );
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
				intermediates[0].BuildLoad64Imm( 32 * sizeof(uint64_t), pc + sizeof(uint32_t) );
				intermediates[1].BuildBranchAlways( target );
				return 2;
			}
			else {
				intermediates[0].BuildBranchAlways( target );
				return 1;
			}
		}

		case OPCD_CMPI: {
			const int ra = D_RA(nativeInstr);
			const int32_t imm = D_SI(nativeInstr);
			const int cr = D_BF(nativeInstr);

			if( D_L(nativeInstr) ) {
				intermediates[0].BuildPpcCmpdi( GPR64OFFSET(ra), cr, imm );
			}
			else {
				intermediates[0].BuildPpcCmpwi( GPR32LOWOFFSET(ra), cr, imm );
			}

			return 1;
		}

		case OPCD_CMPLI: {
			const int ra = D_RA(nativeInstr);
			const uint32_t imm = D_UI(nativeInstr);
			const int cr = D_BF(nativeInstr);

			if( D_L(nativeInstr) ) {
				intermediates[0].BuildPpcCmpldi( GPR64OFFSET(ra), cr, imm );
			}
			else {
				intermediates[0].BuildPpcCmplwi( GPR32LOWOFFSET(ra), cr, imm );
			}

			return 1;
		}

		case OPCD_LWZ: {
			const int rt = D_RT(nativeInstr);
			const int ra = D_RA(nativeInstr);
			int64_t offset = D_D(nativeInstr);

			if( 0 == ra ) {
				intermediates[0].BuildLoad32Abs( GPR32LOWOFFSET(rt), offset );
			}
			else {
				intermediates[0].BuildLoad32RegOffset( GPR32LOWOFFSET(rt), GPR64OFFSET(ra), offset );
			}

			intermediates[1].BuildLoad32Imm( GPR32HIGHOFFSET(rt), 0 );

			return 2;
		}

		case OPCD_ROTATE_21: {
			const int rs = GPR32LOWOFFSET( M_RS(nativeInstr) );
			const int ra = GPR32LOWOFFSET( M_RA(nativeInstr) );

			switch( nativeInstr & 0xFFFF ) {
				case 0x053e: { //clrlwi rs, ra, 20
					intermediates[0].BuildAndImm64( rs, ra, 0x0000000000000FFFUL );
					break;
				}

				case 0x077e: { //clrlwi rs, ra, 29
					intermediates[0].BuildAndImm64( rs, ra, 0x0000000000000007UL );
					break;
				}

				case 0x003a: { //clrrwi rs, ra, 2
					intermediates[0].BuildAndImm64( rs, ra, 0x00000000FFFFFFFCUL );
					break;
				}

				case 0x0038: { //clrrwi rs, ra, 3
					intermediates[0].BuildAndImm64( rs, ra, 0x00000000FFFFFFF8UL );
					break;
				}

				case 0x0026: { //clrrwi rs, ra, 12
					intermediates[0].BuildAndImm64( rs, ra, 0x00000000FFFFF000UL );
					break;
				}

				case 0x482c: { //slwi rs, ra, 9
					intermediates[0].BuildSll32Imm( rs, ra, 9 );
					break;
				}

				case 0x901A: { //slwi rs, ra, 18
					intermediates[0].BuildSll32Imm( rs, ra, 18 );
					break;
				}

				case 0xe8fe: { //srwi rs, ra, 3
					intermediates[0].BuildSlr32Imm( rs, ra, 3 );
					break;
				}

				case 0x843e: { //srwi rs, ra, 16
					intermediates[0].BuildSlr32Imm( rs, ra, 16 );
					break;
				}

				default: {
					intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
					break;
				}
			}
			return 1;
		}

		case OPCD_ROTATE_30: {
			if( MD_RC(nativeInstr) ) {
				intermediates[0].BuildUnknown( 30, nativeInstr, pc );
				return 1;
			}

			const int rs = GPR64OFFSET( D_RS(nativeInstr) );
			const int ra = GPR64OFFSET( D_RA(nativeInstr) );

			const int sh = ((nativeInstr >> 11) & 0x1F) | 
			               ((nativeInstr << 4) & 0x20);

			const int mb = ((nativeInstr >> 6) & 0x1F) |
			               (nativeInstr & 0x20);

			switch( MDS_XO(nativeInstr) ) {
				case 0:
				case 1: {
					uint64_t mask = 0xFFFFFFFFFFFFFFFFUL >> mb;
					if( sh == 0 ) {
						intermediates[0].BuildAndImm64( rs, ra, mask );
						return 1;
					}
					else if( (64 - sh) == mb ) {
						intermediates[0].BuildSlr64Imm( rs, ra, mb );
						return 1;
					}
					else {
						intermediates[0].BuildRol64Imm( rs, ra, sh );
						intermediates[1].BuildAndImm64( ra, ra, mask );
						return 2;
					}
					break;
				}

				case 2:
				case 3: {
					uint64_t mask = 0xFFFFFFFFFFFFFFFFUL << (63 - mb);
					if( sh == 0 ) { //clrrdi
						intermediates[0].BuildAndImm64( rs, ra, mask );
						return 1;
					}
					else if( (63 - mb) == sh ) { //sldi
						intermediates[0].BuildSll64Imm( rs, ra, sh );
						return 1;
					}
					else {
						intermediates[0].BuildRol64Imm( rs, ra, sh );
						intermediates[1].BuildAndImm64( ra, ra, mask );
						return 2;
					}
					break;
				}

				case 6:
				case 7: {
					int numBits = 64 - sh - mb;
					uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - numBits);
					mask <<= sh;

					intermediates[0].BuildAndcImm( ra, ra, mask );
					intermediates[1].BuildRol64Imm( rs, GPR64OFFSET(GPR_TEMP), sh );
					intermediates[2].BuildAndImm64( GPR64OFFSET(GPR_TEMP), GPR64OFFSET(GPR_TEMP), mask );
					intermediates[3].BuildOr( GPR64OFFSET(GPR_TEMP), ra, ra );
					return 4;
				}

				default: {
					printf( "%08x xo=%d\n", nativeInstr, MDS_XO(nativeInstr) );					
					intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
					return 1;
				}
			}
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

		case OPCD_STB: {
			const int ra = D_RA(nativeInstr);
			const int rs = D_RS(nativeInstr);
			
			const int64_t d = D_D(nativeInstr);

			if( 0 == ra ) {
				intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
			}
			else {
				intermediates[0].BuildStore8RegOffset( GPR32LOWOFFSET(rs), GPR64OFFSET(ra), d );
			}
			return 1;
		}

		case OPCD_STW: {
			const int ra = D_RA(nativeInstr);
			const int rs = D_RS(nativeInstr);
			
			const int64_t d = D_D(nativeInstr);

			if( 0 == ra ) {
				intermediates[0].BuildStore32( GPR64OFFSET(rs), d );
			}
			else {
				intermediates[0].BuildStore32RegOffset( GPR32LOWOFFSET(rs), GPR64OFFSET(ra), d );
			}
			return 1;
		}

		case OPCD_STWU: {
			const int ra = D_RA(nativeInstr);
			const int rs = D_RS(nativeInstr);
			
			const int64_t d = D_D(nativeInstr);

			if( 0 == ra ) {
				intermediates[0].BuildInvalid( OPCD(nativeInstr), nativeInstr, pc );
				return 1;
			}

			intermediates[0].BuildAddImm( GPR64OFFSET(ra), GPR64OFFSET(GPR_TEMP), d );
			intermediates[1].BuildStore32Reg( GPR32LOWOFFSET(rs), GPR64OFFSET(GPR_TEMP) );
			intermediates[2].BuildAddImm( GPR64OFFSET(ra), GPR64OFFSET(ra), d );
			return 3;
		}

		case OPCD_LD: {
			const int rs = DS_RS(nativeInstr);
			const int ra = DS_RA(nativeInstr);
			const int64_t offset = DS_DS(nativeInstr);

			//if( 0 == ra ) {
			//	intermediates[0].BuildStore64( GPR64OFFSET(rs), offset );
			//}
			//else {
				intermediates[0].BuildLoad64RegOffset( GPR64OFFSET(rs), GPR64OFFSET(ra), offset );
			//}

			return 1;
		}

		case OPCD_STD: {
			const int rs = DS_RS(nativeInstr);
			const int ra = DS_RA(nativeInstr);
			const int64_t offset = DS_DS(nativeInstr);

			if( 0 == ra ) {
				intermediates[0].BuildStore64( GPR64OFFSET(rs), offset );
			}
			else {
				intermediates[0].BuildStore64RegOffset( GPR64OFFSET(rs), GPR64OFFSET(ra), offset );
			}

			return 1;
		}

		case OPCD_SUBFIC: {
			const int rt = GPR64OFFSET( D_RT(nativeInstr) );
			const int ra = GPR64OFFSET( D_RA(nativeInstr) );
			const int64_t imm = D_SI(nativeInstr);

			intermediates[0].BuildPpcSubfic( rt, ra, imm );

			return 1;
		}

		case OPCD_TABLE_19: {
			return BuildIntermediateTable19( intermediates, nativeInstr, pc );
		}

		case OPCD_XORIS: {
			const int rs = GPR64OFFSET( D_RS(nativeInstr) );
			const int ra = GPR64OFFSET( D_RA(nativeInstr) );
			const uint64_t imm = D_UI(nativeInstr) << 16;

			intermediates[0].BuildXorImm( rs, ra, imm );

			return 1;
		}

		default: {
			intermediates[0].BuildUnknown( opcd, nativeInstr, pc );
			return 1;
		}
	}
}

} //namespace jitpp
