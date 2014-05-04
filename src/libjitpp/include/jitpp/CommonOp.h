#ifndef GEN7_LIBJITPP_COMMONOP_H
#define GEN7_LIBJITPP_COMMONOP_H

#include <cstdint>

namespace jitpp {

struct CommonOp {
	enum Type {
		UNINTIALIZED,
		UNKNOWN,
		INVALID,

		NOP = 100,
		LOAD_REG,

		BRANCH = 200,
	} type;

	CommonOp *next;

	struct Arg {
		enum Type {
			UNINITIALIZED,
			IMM_MISC,
			IMM_U8,
			IMM_U16,
			IMM_U32,
			IMM_U64,
			IMM_F32,
			IMM_F64,
			GPR_8,
			GPR_16,
			GPR_32,
			GPR_64,
			FPR_32,
			FPR_64,
			TMP_8,
			TMP_16,
			TMP_32,
			TMP_64,
			SYSREG,
			ADDR,
		} type;

		union {
			uint8_t  u8;
			uint16_t u16;
			uint32_t u32;
			uint64_t u64;
			float    f32;
			double   f64;
			int      reg;
			int      misc;
			uint64_t addr;
		};

		void SetMisc( int misc ) {
			type = IMM_MISC;
			this->misc = misc;
		}

		void SetU32( uint32_t u32 ) {
			type = IMM_U32;
			this->u32 = u32;
		}

		void SetGpr32( int reg ) {
			type = GPR_32;
			this->reg = reg;
		} 

		void SetAddr( uint64_t addr ) {
			type = ADDR;
			this->addr = addr;
		}

		Arg()
		  : type( UNINITIALIZED )
		{ }
	} args[4];

	CommonOp()
	  : type( UNINTIALIZED )
	  , next( nullptr )
	{ }

	CommonOp( Type type )
	  : type( type )
	  , next( nullptr )
	{ }

	static CommonOp BuildUnknown( int code, int opcode, uint64_t pc ) {
		CommonOp op( UNKNOWN );

		op.args[0].SetMisc( code );
		op.args[1].SetMisc( opcode );
		op.args[2].SetAddr( pc );

		return op;
	}

	static CommonOp BuildInvalid( uint64_t pc ) {
		CommonOp op( INVALID );

		op.args[0].SetAddr( pc );

		return op;
	}

	//build nop

	static CommonOp BuildLoadRegGpr32ImmU32( int targetGpr, uint32_t imm ) {
		CommonOp op( LOAD_REG );

		op.args[0].SetGpr32( targetGpr );
		op.args[1].SetU32( imm );

		return op;
	}

	static CommonOp BuildLoadRegGpr32Gpr32( int targetGpr, int sourceGpr ) {
		CommonOp op( LOAD_REG );

		op.args[0].SetGpr32( targetGpr );
		op.args[1].SetGpr32( sourceGpr );

		return op;
	}

	static CommonOp BuildBranch( uint64_t target ) {
		CommonOp op( BRANCH );

		op.args[0].SetAddr( target );

		return op;
	}
};

} //namespace jitpp

#endif //GEN7_LIBJITPP_COMMONOP_H

