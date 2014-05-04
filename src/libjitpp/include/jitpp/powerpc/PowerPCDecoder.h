#ifndef GEN7_LIBJITPP_POWERPC_POWERPCDECODER_H
#define GEN7_LIBJITPP_POWERPC_POWERPCDECODER_H

#include "util/BitOps.h"

#include <cstdint>

namespace jitpp {

class PowerPCDecoder
{
public:
	static const int OPCD_SHIFT = 26;

	static const uint32_t REG_MASK = 0x1F;
	static const int RA_SHIFT = 16;
	static const int RS_SHIFT = 21;
	static const int RT_SHIFT = 21;

	static const uint32_t B_LK_MASK = 0x00000001;
	static const uint32_t B_AA_MASK = 0x00000002;
	static const uint32_t B_LI_MASK = 0x03FFFFFC;

	static const int B_LI_BITS = 26;

	static const uint32_t D_IMM_MASK = 0x0000FFFF;
	static const int D_IMM_BITS = 16;

	static const int D_L_SHIFT = 21;
	static const uint32_t D_L_MASK = 0x00000001;

	static const int D_BF_SHIFT = 23;
	static const uint32_t D_BF_MASK = 0x00000007;

	static const uint32_t X_XO_MASK = 0x3FF;
	static const int X_XO_SHIFT = 1;

	static const uint32_t XFX_SPR_LO_MASK = 0x01F;
	static const int XFX_SPR_LO_SHIFT = 16;
	static const uint32_t XFX_SPR_HI_MASK = 0x3E0;
	static const int XFX_SPR_HI_SHIFT = 6;

	enum class UnknownCode {
		OPCD,
		SPECIAL,
		UNKNOWN_SPR_READ,
	};

	enum {
		SPR_LR = 8,
	};

	enum {
		OPCD_CMPLI   = 10,
		OPCD_ADDI    = 14,
		OPCD_ADDIS   = 15,
		OPCD_BRANCH  = 18,
		OPCD_ORI     = 24,
		OPCD_SPECIAL = 31,
	};

	enum {
		SPECIAL_XO_MFSPR = 339,
	};

	constexpr int OPCD( const uint32_t instruction ) {
		return instruction >> OPCD_SHIFT;
	}

	constexpr int RA( const uint32_t instruction ) {
		return (instruction >> RA_SHIFT) & REG_MASK;
	}

	constexpr int RS( const uint32_t instruction ) {
		return (instruction >> RS_SHIFT) & REG_MASK;
	}

	constexpr int RT( const uint32_t instruction ) {
		return (instruction >> RT_SHIFT) & REG_MASK;
	}

	constexpr bool B_LK( const uint32_t instruction ) {
		return instruction & B_LK_MASK;
	}

	constexpr bool B_AA( const uint32_t instruction ) {
		return instruction & B_AA_MASK;
	}

	static int64_t B_LI( const uint32_t instruction ) {
		return util::SignExtend<int64_t,B_LI_BITS>( instruction & B_LI_MASK );
	}

	static int16_t D_SI( const uint32_t instruction ) {
		return util::SignExtend<int16_t,D_IMM_BITS>( instruction & D_IMM_MASK );
	}

	constexpr bool D_L( const uint32_t instruction ) {
		return (instruction >> D_L_SHIFT) & D_L_MASK;
	}

	constexpr int D_BF( const uint32_t instruction ) {
		return (instruction >> D_BF_SHIFT) & D_BF_MASK;
	}

	constexpr uint16_t D_UI( const uint32_t instruction ) {
		return instruction & D_IMM_MASK;
	}

	constexpr int X_XO( const uint32_t instruction ) {
		return (instruction >> X_XO_SHIFT) & X_XO_MASK;
	}

	constexpr int XFX_SPR( const uint32_t instruction ) {
		return ((instruction >> XFX_SPR_LO_SHIFT) & XFX_SPR_LO_MASK) | 
		       ((instruction >> XFX_SPR_HI_SHIFT) & XFX_SPR_HI_MASK);
	}

	void DecodeSpecial( uint32_t instr, uint64_t pc );

protected:
	void DecodeInstruction( uint32_t instr, uint64_t pc );

	virtual void OnUnknownInstruction( uint32_t instr, UnknownCode code, 
	                                   int codeArg, uint64_t pc ) = 0;

	virtual void OnAddi( int rt, int ra, int16_t si ) = 0;
	virtual void OnAddis( int rt, int ra, int16_t si ) = 0;
	virtual void OnBranch( uint64_t target, bool link, bool abs ) = 0;
	virtual void OnCmpldi( int bf, int ra, uint16_t ui ) = 0;
	virtual void OnCmplwi( int bf, int ra, uint16_t ui ) = 0;
	virtual void OnLi( int rt, int16_t si ) = 0;
	virtual void OnLis( int rt, int16_t si ) = 0;
	virtual void OnMflr( int rt ) = 0;
	virtual void OnOri( int ra, int rs, uint16_t ui ) = 0;
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_POWERPC_POWERPCDECODER_H

