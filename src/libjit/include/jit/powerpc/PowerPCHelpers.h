#ifndef GEN7_LIBJIT_POWERPC_POWERPCHELPERS_H
#define GEN7_LIBJIT_POWERPC_POWERPCHELPERS_H

#include "jit/Disasm.h"

#include "util/BitOps.h"

namespace jit {

class PowerPCHelpers
{
private:
	static const uint32_t REG_MASK = 0x1F;

	static const int OPCD_SHIFT = 26;

	static const uint32_t B_LK_MASK = 0x00000001;
	static const uint32_t B_AA_MASK = 0x00000002;
	static const uint32_t B_LI_MASK = 0x03FFFFFC;

	static const int B_LI_BITS = 26;

	static const int B_BD_BITS = 16;

	static const uint32_t B_BD_MASK = 0x0000FFFC;

	static const int B_BI_SHIFT = 16;
	static const int B_BO_SHIFT = 21;

	static const uint32_t D_IMM_MASK = 0x0000FFFF;

	static const int D_RA_SHIFT = 16;
	static const int D_RS_SHIFT = 21;
	static const int D_RT_SHIFT = 21;

	static const int D_D_BITS = 16;

	static const uint32_t D_D_MASK = 0x0000FFFF;

	static const int D_L_SHIFT = 21;
	static const uint32_t D_L_MASK = 0x00000001;

	static const int D_BF_SHIFT = 23;
	static const uint32_t D_BF_MASK = 0x00000007;

	static const int D_IMM_BITS = 16;

	static const uint32_t DS_DS_MASK = 0x0000FFFC;
	static const int DS_DS_BITS = 16;

	static const int DS_RA_SHIFT = 16;
	static const int DS_RS_SHIFT = 21;

	static const int M_RA_SHIFT = 16;
	static const int M_RS_SHIFT = 21;

	static const uint32_t MD_RC_MASK = 0x00000001;

	static const uint32_t MDS_XO_MASK = 0x0000000F;

	static const int MDS_XO_SHIFT = 1;

	static const uint32_t X_RC_MASK = 0x000000001;

	static const int X_RB_SHIFT = 11;
	static const int X_RA_SHIFT = 16;
	static const int X_RS_SHIFT = 21;
	static const int X_RT_SHIFT = 21;

	static const int X_L_SHIFT = 21;
	static const uint32_t X_L_MASK = 0x00000001;

	static const uint32_t X_BF_MASK = 0x07;
	static const uint32_t X_BF_SHIFT = 23;

	static const uint32_t X_XO_MASK = 0x3FF;
	static const int X_XO_SHIFT = 1;

	static const uint32_t XFX_SPR_LO_MASK = 0x01F;
	static const int XFX_SPR_LO_SHIFT = 16;
	static const uint32_t XFX_SPR_HI_MASK = 0x3E0;
	static const int XFX_SPR_HI_SHIFT = 6;

	static const uint32_t XFX_FXM_MASK = 0xFF;
	static const int XFX_FXM_SHIFT = 12;

	static const int XFX_RS_SHIFT = 21;
	static const int XFX_RT_SHIFT = 21;

	static const int XO_RB_SHIFT = 11;
	static const int XO_RA_SHIFT = 16;
	static const int XO_RS_SHIFT = 21;
	static const int XO_RT_SHIFT = 21;

	static const uint32_t XO_RC_MASK = 0x00000001;

public:
	enum {
		//Fake GPRs for intermediates
		GPR_LR    = 32,
		GPR_CTR   = 33,
		GPR_SPRG0 = 34,
		GPR_SPRG1 = 35,
		GPR_SPRG2 = 36,
		GPR_SPRG3 = 37,
		GPR_CR    = 38,
		GPR_XER   = 39,
		GPR_TEMP  = 40,
	};

	enum {
		SPR_XER                = 1,
		SPR_LR                 = 8,
		SPR_CTR                = 9,
		SPR_DAR                = 19,
		SPR_SRR0               = 26,
		SPR_SRR1               = 27,
		SPR_SPRG0              = 272,
		SPR_SPRG1              = 273,
		SPR_SPRG2              = 274,
		SPR_SPRG3              = 275,
		SPR_HRMOR              = 313,
		SPR_LPCR               = 318,
		SPR_LPIDR              = 319,
		SPR_PPE_TLB_INDEX_HINT = 946,
		SPR_PPE_TLB_INDEX      = 947,
		SPR_PPE_TLB_VPN        = 948,
		SPR_PPE_TLB_RPN        = 949,
		SPR_HID6               = 1017,
		SPR_PIR                = 1023,

		//Fake SPRs for intermediates
		SPR_MSR   = 1024,
	};

	enum {
		OPCD_RESERVED_0 = 0,
		OPCD_CMPLI      = 10,
		OPCD_CMPI       = 11,
		OPCD_ADDI       = 14,
		OPCD_ADDIS      = 15,
		OPCD_BCC        = 16,
		OPCD_BRANCH     = 18,
		OPCD_TABLE_19   = 19,
		OPCD_ROTATE_21  = 21,
		OPCD_ORI        = 24,
		OPCD_ORIS       = 25,
		OPCD_ROTATE_30  = 30,
		OPCD_SPECIAL    = 31,
		OPCD_LWZ        = 32,
		OPCD_STW        = 36,
		OPCD_STWU       = 37,
		OPCD_LD         = 58,
		OPCD_STD        = 62,
	};

	enum {
		SPECIAL_XO_MFCR   = 19,
		SPECIAL_XO_LWARX  = 20,
		SPECIAL_XO_SLW    = 24,
		SPECIAL_XO_CMPL   = 32,
		SPECIAL_XO_SUBF   = 40,
		SPECIAL_XO_ANDC   = 60,
		SPECIAL_XO_MFMSR  = 83,
		SPECIAL_XO_MTCRF  = 144,
		SPECIAL_XO_STWCX  = 150,
		SPECIAL_XO_ADD    = 266,
		SPECIAL_XO_TLBIEL = 274,
		SPECIAL_XO_MFSPR  = 339,
		SPECIAL_XO_SLBMTE = 402,
		SPECIAL_XO_OR     = 444,
		SPECIAL_XO_MTSPR  = 467,
		SPECIAL_XO_SLBIA  = 498,
		SPECIAL_XO_SYNC   = 598,
		SPECIAL_XO_EIEIO  = 854,
	};

	enum {
		TABLE_19_XO_BCLR  = 16,
		TABLE_19_XO_RFID  = 18,
		TABLE_19_XO_ISYNC = 150,
		TABLE_19_XO_BCCTR = 528,
	};

	static int OPCD( const uint32_t instruction ) {
		return instruction >> OPCD_SHIFT;
	}

	static bool B_LK( const uint32_t instruction ) {
		return instruction & B_LK_MASK;
	}

	static bool B_AA( const uint32_t instruction ) {
		return instruction & B_AA_MASK;
	}

	static int64_t B_LI( const uint32_t instruction ) {
		return util::SignExtend<int64_t,B_LI_BITS>( instruction & B_LI_MASK );
	}

	static int64_t B_BD( const uint32_t instruction ) {
		return util::SignExtend<int64_t,B_BD_BITS>( instruction & B_BD_MASK );
	}

	static int B_BI( const uint32_t instruction ) {
		return (instruction >> B_BI_SHIFT) & REG_MASK;
	}

	static int B_BO( const uint32_t instruction ) {
		return (instruction >> B_BO_SHIFT) & REG_MASK;
	}

	static int64_t D_D( const uint32_t instruction ) {
		return util::SignExtend<int64_t,D_D_BITS>( instruction & D_D_MASK );
	}

	static int64_t D_SI( const uint32_t instruction ) {
		return util::SignExtend<int64_t,D_IMM_BITS>( instruction & D_IMM_MASK );
	}

	static uint64_t D_UI( const uint32_t instruction ) {
		return instruction & D_IMM_MASK;
	}

	static int D_RA( const uint32_t instruction ) {
		return (instruction >> D_RA_SHIFT) & REG_MASK;
	}

	static int D_RS( const uint32_t instruction ) {
		return (instruction >> D_RS_SHIFT) & REG_MASK;
	}

	static int D_RT( const uint32_t instruction ) {
		return (instruction >> D_RT_SHIFT) & REG_MASK;
	}

	static bool D_L( const uint32_t instruction ) {
		return (instruction >> D_L_SHIFT) & D_L_MASK;
	}

	static int D_BF( const uint32_t instruction ) {
		return (instruction >> D_BF_SHIFT) & D_BF_MASK;
	}

	static int64_t DS_DS( const uint32_t instruction ) {
		return util::SignExtend<int64_t,DS_DS_BITS>( instruction & DS_DS_MASK );
	}

	static int DS_RA( const uint32_t instruction ) {
		return (instruction >> DS_RA_SHIFT) & REG_MASK;
	}

	static int DS_RS( const uint32_t instruction ) {
		return (instruction >> DS_RS_SHIFT) & REG_MASK;
	}

	static int M_RA( const uint32_t instruction ) {
		return (instruction >> M_RA_SHIFT) & REG_MASK;
	}

	static int M_RS( const uint32_t instruction ) {
		return (instruction >> M_RS_SHIFT) & REG_MASK;
	}

	static bool MD_RC( const uint32_t instruction ) {
		return instruction & MD_RC_MASK;
	}

	static int MDS_XO( const uint32_t instruction ) {
		return (instruction >> MDS_XO_SHIFT) & MDS_XO_MASK;
	}

	static bool X_RC( const uint32_t instruction ) {
		return instruction & X_RC_MASK;
	}

	static int X_XO( const uint32_t instruction ) {
		return (instruction >> X_XO_SHIFT) & X_XO_MASK;
	}

	static int X_RB( const uint32_t instruction ) {
		return (instruction >> X_RB_SHIFT) & REG_MASK;
	}

	static int X_RA( const uint32_t instruction ) {
		return (instruction >> X_RA_SHIFT) & REG_MASK;
	}

	static int X_RS( const uint32_t instruction ) {
		return (instruction >> X_RS_SHIFT) & REG_MASK;
	}

	static int X_RT( const uint32_t instruction ) {
		return (instruction >> X_RT_SHIFT) & REG_MASK;
	}

	static bool X_L( const uint32_t instruction ) {
		return (instruction >> X_L_SHIFT) & X_L_MASK;
	}

	static int X_BF( const uint32_t instruction ) {
		return (instruction >> X_BF_SHIFT) & X_BF_MASK;
	}

	static int XFX_SPR( const uint32_t instruction ) {
		return ((instruction >> XFX_SPR_LO_SHIFT) & XFX_SPR_LO_MASK) | 
		       ((instruction >> XFX_SPR_HI_SHIFT) & XFX_SPR_HI_MASK);
	}

	static int XFX_FXM( const uint32_t instruction ) {
		return (instruction >> XFX_FXM_SHIFT) & XFX_FXM_MASK;
	}

	static int XFX_RS( const uint32_t instruction ) {
		return (instruction >> XFX_RS_SHIFT) & REG_MASK;
	}

	static int XFX_RT( const uint32_t instruction ) {
		return (instruction >> XFX_RT_SHIFT) & REG_MASK;
	}

	static bool XO_RC( const uint32_t instruction ) {
		return instruction & XO_RC_MASK;
	}

	static int XO_RB( const uint32_t instruction ) {
		return (instruction >> XO_RB_SHIFT) & REG_MASK;
	}

	static int XO_RA( const uint32_t instruction ) {
		return (instruction >> XO_RA_SHIFT) & REG_MASK;
	}

	static int XO_RS( const uint32_t instruction ) {
		return (instruction >> XO_RS_SHIFT) & REG_MASK;
	}

	static int XO_RT( const uint32_t instruction ) {
		return (instruction >> XO_RT_SHIFT) & REG_MASK;
	}

};

} //namespace jit

#endif //GEN7_LIBJIT_POWERPC_POWERPCHELPERS_H

