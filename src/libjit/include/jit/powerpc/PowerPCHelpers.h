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

	static const uint32_t D_IMM_MASK = 0x0000FFFF;

	static const int D_RA_SHIFT = 16;
	static const int D_RS_SHIFT = 21;
	static const int D_RT_SHIFT = 21;

	static const int D_IMM_BITS = 16;

	static const uint32_t X_XO_MASK = 0x3FF;

	static const int X_XO_SHIFT = 1;

	static const uint32_t XFX_SPR_LO_MASK = 0x01F;
	static const int XFX_SPR_LO_SHIFT = 16;
	static const uint32_t XFX_SPR_HI_MASK = 0x3E0;
	static const int XFX_SPR_HI_SHIFT = 6;

	static const uint32_t XFX_RS_MASK = 0x1f;
	static const int XFX_RS_SHIFT = 21;

public:
	enum {
		//Fake GPRs for intermediates
		GPR_LR    = 32,
		GPR_CTR   = 33,
		GPR_SPRG0 = 34,
		GPR_SPRG1 = 35,
		GPR_SPRG2 = 36,
		GPR_SPRG3 = 37,
	};

	enum {
		SPR_LR    = 8,
		SPR_CTR   = 9,
		SPR_SRR1  = 27,
		SPR_SPRG0 = 272,
		SPR_SPRG1 = 273,
		SPR_SPRG2 = 274,
		SPR_SPRG3 = 275,
		SPR_HRMOR = 313,
		SPR_LPCR  = 318,
		SPR_LPIDR = 319,
		SPR_HID6  = 1017,
		SPR_PIR   = 1023,

		//Fake SPRs for intermediates
		SPR_MSR   = 1024,
	};

	enum {
		OPCD_RESERVED_0 = 0,
		OPCD_ADDI       = 14,
		OPCD_ADDIS      = 15,
		OPCD_BRANCH     = 18,
		OPCD_ORI        = 24,
		OPCD_ORIS       = 25,
		OPCD_ROTATE     = 30,
		OPCD_SPECIAL    = 31,
	};

	enum {
		SPECIAL_XO_MTSPR = 467,
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

	static int X_XO( const uint32_t instruction ) {
		return (instruction >> X_XO_SHIFT) & X_XO_MASK;
	}

	static int XFX_SPR( const uint32_t instruction ) {
		return ((instruction >> XFX_SPR_LO_SHIFT) & XFX_SPR_LO_MASK) | 
		       ((instruction >> XFX_SPR_HI_SHIFT) & XFX_SPR_HI_MASK);
	}

	static int XFX_RS( const uint32_t instruction ) {
		return (instruction >> XFX_RS_SHIFT) & XFX_RS_MASK;
	}

};

} //namespace jit

#endif //GEN7_LIBJIT_POWERPC_POWERPCHELPERS_H

