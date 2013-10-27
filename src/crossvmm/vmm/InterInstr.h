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

	InterInstr()
	  : op( UNKNOWN_OPCODE )
	  , rsvd( 0 )
	{ }

	void BuildUnknown( int opcode, uint64_t instruction, uint64_t pc ) {
		op = UNKNOWN_OPCODE;
		args[0] = opcode;
		args[1] = instruction;
		args[2] = pc;
	}

	void BuildNop() {
		op = NOP;
	}

	void BuildSetSystemImm( uint64_t value, int sysReg ) {
		op = SET_SYS_IMM;
		args[0] = value;
		args[1] = sysReg;
	}

	void BuildSetSystemReg( int sourceReg, int sysReg ) {
		op = SET_SYS_REG;
		args[0] = sourceReg;
		args[1] = sysReg;
	}

	void BuildReadSystem( int destReg, int sysReg ) {
		op = READ_SYS;
		args[0] = destReg;
		args[1] = sysReg;
	}

	void BuildMoveReg( int sourceReg, int destReg ) {
		op = MOVE_REG;
		args[0] = sourceReg;
		args[1] = destReg;
	}

	void BuildBranchAlways( uint64_t target ) {
		op = BRANCH_ALWAYS;
		args[0] = target;
	}

	void BuildLoadImm( int destReg, uint64_t value ) {
		op = LOAD_IMM;
		args[0] = destReg;
		args[1] = value;
	}

	void BuildAndc( int sourceReg0, int sourceReg1, int destReg ) {
		op = ANDC;
		args[0] = sourceReg0;
		args[1] = sourceReg1;
		args[2] = destReg;
	}

	void BuildOr( int sourceReg0, int sourceReg1, int destReg ) {
		op = OR;
		args[0] = sourceReg0;
		args[1] = sourceReg1;
		args[2] = destReg;
	}

	void BuildOrImm( int sourceReg, int destReg, uint64_t imm ) {
		op = OR_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = imm;
	}

	void BuildSll64( int sourceReg, int destReg, int shift ) {
		op = SLL64;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = shift;
	}

	void BuildPpcSlbia() {
		op = PPC_SLBIA;
	}

	//SLBMTE
};

#endif //GEN7_CROSSVMM_INTERINSTR_H

