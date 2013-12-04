#ifndef GEN7_LIBJIT_INTERINSTR_H
#define GEN7_LIBJIT_INTERINSTR_H

#include <cstdint>

namespace jit {

enum InstrOp
{
	//Misc
	UNKNOWN_OPCODE = 0,
	NOP,
	SET_SYS_IMM,
	SET_SYS_REG,
	READ_SYS,
	MOVE_REG32,
	MOVE_REG64,

	//Branch
	BRANCH_ALWAYS = 100,
	BRANCH_GPR64,
	BRANCH_GPR32_MASK_ZERO,
	BRANCH_GPR32_MASK_NOT_ZERO,
	BRANCH_GPR64_NOT_ZERO,

	//Load/Store
	LD_32_IMM = 200,
	LD_64_IMM,
	LD_32,
	LD_32_REG_OFF,
	LD_32_L,
	LD_32_IDX_L,
	ST_64,
	ST_64_REG_OFF,

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
	ROL64_IMM = 500,
	SLL32,
	SLL32_IMM,
	SLL64_IMM,
	SLR64_IMM,

	//Processor Specific Region
	PROC_LOW = 10000,

	//PowerPC specific
	PPC_SLBIA = PROC_LOW,
	PPC_SLBMTE,
	PPC_TLBIEL,
	PPC_STWCX,
	PPC_CMPLWI,
	PPC_CMPLDI,
	PPC_RFID,
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

	void BuildMoveReg32( int sourceReg, int destReg ) {
		op = MOVE_REG32;
		args[0] = sourceReg;
		args[1] = destReg;
	}

	void BuildMoveReg64( int sourceReg, int destReg ) {
		op = MOVE_REG64;
		args[0] = sourceReg;
		args[1] = destReg;
	}

//Branch
	void BuildBranchAlways( uint64_t target ) {
		op = BRANCH_ALWAYS;
		args[0] = target;
	}

	void BuildBranchGpr64( int gpr ) {
		op = BRANCH_GPR64;
		args[0] = gpr;
	}

	void BuildBranchGpr32MaskZero( int gpr, uint32_t mask, uint64_t target ) {
		op = BRANCH_GPR32_MASK_ZERO;
		args[0] = gpr;
		args[1] = mask;
		args[2] = target;
	}

	void BuildBranchGpr32MaskNotZero( int gpr, uint32_t mask, uint64_t target ) {
		op = BRANCH_GPR32_MASK_NOT_ZERO;
		args[0] = gpr;
		args[1] = mask;
		args[2] = target;
	}

	void BuildBranchGpr64NotZero( int gpr, uint64_t target ) {
		op = BRANCH_GPR64_NOT_ZERO;
		args[0] = gpr;
		args[1] = target;
	}

//Load/Store
	void BuildLoad32Imm( int destReg, uint64_t value ) {
		op = LD_32_IMM;
		args[0] = destReg;
		args[1] = value;
	}

	void BuildLoad64Imm( int destReg, uint64_t value ) {
		op = LD_64_IMM;
		args[0] = destReg;
		args[1] = value;
	}

	void BuildLoad32( int destReg, uint64_t addr ) {
		op = LD_32;
		args[0] = destReg;
		args[1] = addr;
	}

	void BuildLoad32RegOffset( int destReg, int addrReg, int64_t offset ) {
		op = LD_32_REG_OFF;
		args[0] = destReg;
		args[1] = addrReg;
		args[2] = offset;
	}

	void BuildLoad32Linked( int sourceReg, int destReg ) {
		op = LD_32_L;
		args[0] = sourceReg;
		args[1] = destReg;
	}

	void BuildLoad32IndexedLinked( int sourceReg, int offsetReg, int destReg ) {
		op = LD_32_IDX_L;
		args[0] = sourceReg;
		args[1] = offsetReg;
		args[2] = destReg;
	}

	void BuildStore64( int sourceReg, uint64_t addr ) {
		op = ST_64;
		args[0] = sourceReg;
		args[1] = addr;
	}

	void BuildStore64RegOffset( int sourceReg, int addrReg, int64_t offset ) {
		op = ST_64_REG_OFF;
		args[0] = sourceReg;
		args[1] = addrReg;
		args[2] = offset;
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
	void BuildRol64Imm( int sourceReg, int destReg, int rotate ) {
		op = ROL64_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = rotate;
	}

	void BuildSll32( int sourceReg, int destReg, int shiftReg ) {
		op = SLL32;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = shiftReg;
	}

	void BuildSll32Imm( int sourceReg, int destReg, int shift ) {
		op = SLL32_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = shift;
	}

	void BuildSll64Imm( int sourceReg, int destReg, int shift ) {
		op = SLL64_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = shift;
	}

	void BuildSlr64Imm( int sourceReg, int destReg, int shift ) {
		op = SLR64_IMM;
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

	void BuildPpcTlbiel( int rb, bool l ) {
		op = PPC_TLBIEL;
		args[0] = rb;
		args[1] = l;
	}

	void BuildPpcStwcx( int sourceReg, int offsetReg, int destReg ) {
		op = PPC_STWCX;
		args[0] = sourceReg;
		args[1] = offsetReg;
		args[2] = destReg;
	}

	void BuildPpcCmplwi( int reg, int cr, uint32_t imm ) {
		op = PPC_CMPLWI;
		args[0] = reg;
		args[1] = cr;
		args[2] = imm;
	}

	void BuildPpcCmpldi( int reg, int cr, uint32_t imm ) {
		op = PPC_CMPLDI;
		args[0] = reg;
		args[1] = cr;
		args[2] = imm;
	}

	void BuildPpcRfid() {
		op = PPC_RFID;
	}
};

} //namespace jit

#endif //GEN7_LIBJIT_INTERINSTR_H

