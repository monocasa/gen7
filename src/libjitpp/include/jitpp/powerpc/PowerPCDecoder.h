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

	enum class UnknownCode {
		OPCD,
	};

	enum {
		OPCD_ADDI   = 14,
		OPCD_BRANCH = 18,
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

	constexpr uint16_t D_UI( const uint32_t instruction ) {
		return instruction & D_IMM_MASK;
	}

protected:
	void DecodeInstruction( uint32_t instr, uint64_t pc );

	virtual void OnUnknownInstruction( uint32_t instr, UnknownCode code, 
	                                   int codeArg, uint64_t pc ) = 0;

	virtual void OnAddi( int rt, int ra, int16_t si ) = 0;
	virtual void OnBranch( uint64_t target, bool link, bool abs ) = 0;
	virtual void OnLi( int rt, int16_t si ) = 0;
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_POWERPC_POWERPCDECODER_H

