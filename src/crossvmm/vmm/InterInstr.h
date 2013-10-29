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
	BRANCH_GPR_NOT_ZERO,

	//Load/Store
	LOAD_IMM = 200,

	//Arithmetic
	ADD = 300,
	ADD_IMM,
	SUB,
	SUBU_IMM,

	//Logic
	AND_IMM = 400,
	ANDC,
	OR,
	OR_IMM,

	//Shift
	SLL32 = 500,
	SLL64,

	//Processor Specific Region
	PROC_LOW = 10000,

	//PowerPC specific
	PPC_SLBIA = PROC_LOW,
	PPC_SLBMTE,
	PPC_TLBIEL,
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

//Misc
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

//Branch
	void BuildBranchAlways( uint64_t target ) {
		op = BRANCH_ALWAYS;
		args[0] = target;
	}

	void BuildBranchGprNotZero( int gpr, uint64_t target ) {
		op = BRANCH_GPR_NOT_ZERO;
		args[0] = gpr;
		args[1] = target;
	}

//Load/Store
	void BuildLoadImm( int destReg, uint64_t value ) {
		op = LOAD_IMM;
		args[0] = destReg;
		args[1] = value;
	}

//Arithmetic
	void BuildAdd( int sourceReg0, int sourceReg1, int destReg ) {
		op = ADD;
		args[0] = sourceReg0;
		args[1] = sourceReg1;
		args[2] = destReg;
	}

	void BuildAddImm( int sourceReg, int destReg, uint64_t imm ) {
		op = ADD_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = imm;
	}

	void BuildSub( int sourceReg0, int sourceReg1, int destReg ) {
		op = SUB;
		args[0] = sourceReg0;
		args[1] = sourceReg1;
		args[2] = destReg;
	}

	void BuildSubuImm( int sourceReg, int destReg, uint64_t imm ) {
		op = SUBU_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = imm;
	}

//Logic
	void BuildAndImm( int sourceReg, int destReg, uint64_t imm ) {
		op = AND_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = imm;
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

//Shift/Rotate
	void BuildSll32( int sourceReg, int destReg, int shift ) {
		op = SLL32;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = shift;
	}

	void BuildSll64( int sourceReg, int destReg, int shift ) {
		op = SLL64;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = shift;
	}

//PowerPC Specific
	void BuildPpcSlbia() {
		op = PPC_SLBIA;
	}

	void BuildPpcSlbmte( int rs, int rb ) {
		op = PPC_SLBMTE;
		args[0] = rs;
		args[1] = rb;
	}

	void BuildPpcTlbiel( int rb ) {
		op = PPC_TLBIEL;
		args[0] = rb;
	}
};

#endif //GEN7_CROSSVMM_INTERINSTR_H

