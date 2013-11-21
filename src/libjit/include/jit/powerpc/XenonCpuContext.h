#ifndef GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H
#define GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H

#include "jit/CpuContext.h"

#include <cstdint>

namespace jit {

struct VmxReg {
	uint8_t bytes[ 16 ];
};

struct ConditionRegister {
	union {
		struct {
			uint8_t so : 1;
			uint8_t eq : 1;
			uint8_t gt : 1;
			uint8_t lt : 1;
		};
		uint8_t nybble : 4;
	};
};

struct XenonCpuContext : public CpuContext {
	uint64_t gpr[32];
	uint64_t lr;
	uint64_t ctr;
	uint64_t sprg0;
	uint64_t sprg1;
	uint64_t sprg2;
	uint64_t sprg3;

	uint32_t cr;

	uint64_t fpr[32];
	VmxReg   vmx[128];

	uint64_t pc;

	bool isReserved;
	uint64_t reservation;

	uint64_t msr;
	uint64_t hrmor;
	uint64_t srr0;
	uint64_t srr1;
	uint64_t hid6;
	uint64_t lpidr;
	uint64_t lpcr;

	void ClearCr( int num ) {
		unsigned int mask = 0xF0000000U >> (num * 4);
		cr &= ~mask;
	}

	unsigned int ReadCr( int num ) {
		return (cr >> ((7 - num) * 4)) & 0xF;
	}

	void SetCrSo( int num ) {
		cr |= (0x10000000 >> (num * 4));
	}

	void SetCrEq( int num ) {
		cr |= (0x20000000 >> (num * 4));
	}

	void SetCrGt( int num ) {
		cr |= (0x40000000 >> (num * 4));
	}

	void SetCrLt( int num ) {
		cr |= (0x80000000 >> (num * 4));
	}

	void ClearCrSo( int num ) {
		cr &= ~(0x10000000 >> (num * 4));
	}

	void ClearCrEq( int num ) {
		cr &= ~(0x20000000 >> (num * 4));
	}

	void ClearCrGt( int num ) {
		cr &= ~(0x40000000 >> (num * 4));
	}

	void ClearCrLt( int num ) {
		cr &= ~(0x80000000 >> (num * 4));
	}

	XenonCpuContext( int coreNum )
	  : CpuContext( jit::CpuType::XENON, coreNum )
	{ }
};

} //namespace jit

#endif //GEN7_LIBJIT_POWERPC_XENONCPUCONTEXT_H

