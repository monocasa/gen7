#ifndef GEN7_LIBJIT_POWERPC_POWERPCHELPERS_H
#define GEN7_LIBJIT_POWERPC_POWERPCHELPERS_H

#include "jit/Disasm.h"

#include "shared/Util.h"

namespace jit {

class PowerPCHelpers
{
private:
	static const uint32_t B_LK_MASK = 0x00000001;
	static const uint32_t B_AA_MASK = 0x00000002;
	static const uint32_t B_LI_MASK = 0x03FFFFFC;

	static const int B_LI_BITS = 26;

	static const int OPCD_SHIFT = 26;

public:
	enum {
		OPCD_RESERVED_0 = 0,
		OPCD_BRANCH     = 18,
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
};

} //namespace jit

#endif //GEN7_LIBJIT_POWERPC_POWERPCHELPERS_H

