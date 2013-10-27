#ifndef GEN7_CROSSVMM_INTERINSTR_H
#define GEN7_CROSSVMM_INTERINSTR_H

#include <cstdint>

enum InstrOp
{
	//Misc
	UNKNOWN_OPCODE = 0,
	NOP,
	SET_SYS_IMM,
	SET_SYS_REG,
	READ_SYS,
	MOVE_REG,

	//Branch
	BRANCH_ALWAYS = 100,

	//Load/Store
	LOAD_IMM = 200,

	//Logic
	ANDC = 300,
	OR,
	OR_IMM,

	//Shift
	SLL64 = 400,

	//Processor Specific Region
	PROC_LOW = 10000,

	//PowerPC specific
	PPC_SLBIA = PROC_LOW,
	PPC_SLBMTE,
};

struct InterInstr
{
	uint32_t op;
	uint32_t rsvd;
	uint64_t args[4];

	InterInstr( InstrOp op )
	  : op( op )
	  , rsvd( 0 )
	{ }

	InterInstr( InstrOp op, uint64_t arg0 )
	  : op( op )
	  , rsvd( 0 )
	{
		args[0] = arg0;
	}

	InterInstr( InstrOp op, uint64_t arg0, uint64_t arg1 )
	  : op( op )
	  , rsvd( 0 )
	{
		args[0] = arg0;
		args[1] = arg1;
	}

	InterInstr( InstrOp op, uint64_t arg0, uint64_t arg1, uint64_t arg2 )
	  : op( op )
	  , rsvd( 0 )
	{
		args[0] = arg0;
		args[1] = arg1;
		args[2] = arg2;
	}

	InterInstr( InstrOp op, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3 )
	  : op( op )
	  , rsvd( 0 )
	{
		args[0] = arg0;
		args[1] = arg1;
		args[2] = arg2;
		args[3] = arg3;
	}
};

#endif //GEN7_CROSSVMM_INTERINSTR_H

