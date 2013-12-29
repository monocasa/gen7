#ifndef GEN7_LIBJIT_INTERINSTR_H
#define GEN7_LIBJIT_INTERINSTR_H

#include <cstdint>

namespace jit {

enum InstrOp
{
	//Misc
	UNKNOWN_OPCODE = 0,
	INVALID_OPCODE,
	NOP,
	MOVE_REG,

	//Branch
	BRANCH_ALWAYS = 100,
	BRANCH_GPR64,
	BRANCH_GPR32_MASK_ZERO,
	BRANCH_GPR32_MASK_NOT_ZERO,
	BRANCH_GPR64_NOT_ZERO,
	BRANCH_GPR64_NOT_ZERO_GPR64,

	//Load/Store
	LD_32_IMM = 200,
	LD_64_IMM,
	LD_32_ABS,
	LD_32_REG_OFF,
	LD_64_REG_OFF,
	LD_32_L,
	ST_32,
	ST_64,
	ST_8_REG,
	ST_32_REG,
	ST_64_REG_OFF,

	//Arithmetic
	ADD = 300,
	ADD_IMM,
	SUB,
	SUBU_IMM,

	//Logic
	AND_IMM_32 = 400,
	AND_IMM_64,
	ANDC,
	ANDC_IMM,
	OR,
	OR_IMM,
	XOR_IMM,

	//Shift
	ROL32_IMM = 500,
	ROL64_IMM,
	SLL32,
	SLL32_IMM,
	SLL64_IMM,
	SLR32_IMM,
	SLR64_IMM,
	BYTE_SWAP_32,

	//Processor Specific Region
	PROC_LOW = 10000,

	//PowerPC specific
	PPC_SLBIA = PROC_LOW,
	PPC_SLBMTE,
	PPC_TLBIEL,
	PPC_STWCX,
	PPC_CMPD,
	PPC_CMPDI,
	PPC_CMPW,
	PPC_CMPWI,
	PPC_CMPLD,
	PPC_CMPLDI,
	PPC_CMPLW,
	PPC_CMPLWI,
	PPC_RFID,
	PPC_SRAWI,
	PPC_SUBFIC,
};

enum class OpType {
	UNDEFINED,
	IMM,
	GPR32,
	GPR64,
	SYS32,
	SYS64,
	GPRADDR,
};

template<typename T>
struct Operand
{
public:
	OpType type;

private:
	T value;

public:
	template<OpType opType>
	void Set( T newValue ) {
		type = opType;
		value = newValue;
	}

	const T& operator*() const {
		return value;
	}

	Operand()
	  : type( OpType::UNDEFINED )
	  , value( 0 )
	{ }
};

struct UnknownArgs {
	Operand<int>      opcodeCookie;
	Operand<uint64_t> instruction;
	Operand<uint64_t> pc;
};

template<typename T>
struct TwoOpArgs {
	Operand<int> dest;
	Operand<T>   source;
};

template<typename T, typename U>
struct ThreeOpArgs {
	Operand<int> dest;
	Operand<T>   source1;
	Operand<U>   source2;
};

struct LoadStoreIndexedArgs {
	Operand<int>     valueReg;
	Operand<int>     addrReg;
	Operand<int64_t> offset;
};

struct InterInstr
{
	uint32_t op;

	union {
		uint64_t args[4];
	};

	union {
		UnknownArgs          unknown;
		TwoOpArgs<uint32_t>  imm32;
		TwoOpArgs<uint64_t>  imm64;
		TwoOpArgs<int>       twoReg;
		ThreeOpArgs<int,int> threeReg;
		LoadStoreIndexedArgs ldStIdx;
	};

	InterInstr()
	  : op( UNKNOWN_OPCODE )
	{ }

//Misc
	void BuildUnknown( int opcode, uint64_t instruction, uint64_t pc ) {
		op = UNKNOWN_OPCODE;
		unknown.opcodeCookie.Set<OpType::IMM>( opcode );
		unknown.instruction.Set<OpType::IMM>( instruction );
		unknown.pc.Set<OpType::IMM>( pc );
	}

	void BuildInvalid( int opcode, uint64_t instruction, uint64_t pc ) {
		op = INVALID_OPCODE;
		unknown.opcodeCookie.Set<OpType::IMM>( opcode );
		unknown.instruction.Set<OpType::IMM>( instruction );
		unknown.pc.Set<OpType::IMM>( pc );
	}

	void BuildNop() {
		op = NOP;
	}

	void BuildSetSystemImm( int sysReg, uint64_t value ) {
		op = LD_64_IMM;
		imm64.dest.Set<OpType::SYS64>( sysReg );
		imm64.source.Set<OpType::IMM>( value );
	}

	void BuildSetSystemReg( int destSys, int sourceGpr ) {
		op = MOVE_REG;
		twoReg.dest.Set<OpType::SYS64>( destSys );
		twoReg.source.Set<OpType::GPR64>( sourceGpr );
	}

	void BuildReadSystem( int destGpr, int sourceSys ) {
		op = MOVE_REG;
		twoReg.dest.Set<OpType::GPR64>( destGpr );
		twoReg.source.Set<OpType::SYS64>( sourceSys );
	}

	void BuildMoveReg32( int destGpr, int sourceGpr ) {
		op = MOVE_REG;
		twoReg.dest.Set<OpType::GPR32>( destGpr );
		twoReg.source.Set<OpType::GPR32>( sourceGpr );
	}

	void BuildMoveReg64( int destGpr, int sourceGpr ) {
		op = MOVE_REG;
		twoReg.dest.Set<OpType::GPR64>( destGpr );
		twoReg.source.Set<OpType::GPR64>( sourceGpr );
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

	void BuildBranchGpr64NotZeroGpr64( int gpr, uint64_t targetReg ) {
		op = BRANCH_GPR64_NOT_ZERO_GPR64;
		args[0] = gpr;
		args[1] = targetReg;
	}

//Load/Store
	void BuildLoad32Imm( int destReg, uint32_t value ) {
		op = LD_32_IMM;
		imm32.dest.Set<OpType::GPR32>( destReg );
		imm32.source.Set<OpType::IMM>( value );
	}

	void BuildLoad64Imm( int destReg, uint64_t value ) {
		op = LD_64_IMM;
		imm64.dest.Set<OpType::GPR64>( destReg );
		imm64.source.Set<OpType::IMM>( value );
	}

	void BuildLoad32Abs( int destReg, uint64_t addr ) {
		op = LD_32_ABS;
		imm64.dest.Set<OpType::GPR32>( destReg );
		imm64.source.Set<OpType::IMM>( addr );
	}

	void BuildLoad32RegOffset( int destReg, int addrReg, int64_t offset ) {
		op = LD_32_REG_OFF;
		ldStIdx.valueReg.Set<OpType::GPR32>( destReg );
		ldStIdx.addrReg.Set<OpType::GPR64>( addrReg );
		ldStIdx.offset.Set<OpType::IMM>( offset );
	}

	void BuildLoad64RegOffset( int destReg, int addrReg, int64_t offset ) {
		op = LD_64_REG_OFF;
		ldStIdx.valueReg.Set<OpType::GPR64>( destReg );
		ldStIdx.addrReg.Set<OpType::GPR64>( addrReg );
		ldStIdx.offset.Set<OpType::IMM>( offset );
	}

	void BuildLoad32Linked( int destReg, int sourceReg ) {
		op = LD_32_L;
		twoReg.dest.Set<OpType::GPR32>( destReg );
		twoReg.source.Set<OpType::GPR64>( sourceReg );
	}

	void BuildStore32( int sourceReg, uint64_t addr ) {
		op = ST_32;
		args[0] = sourceReg;
		args[1] = addr;
	}

	void BuildStore64( int sourceReg, uint64_t addr ) {
		op = ST_64;
		args[0] = sourceReg;
		args[1] = addr;
	}

	void BuildStore8Reg( int sourceReg, int addrReg ) {
		op = ST_8_REG;
		args[0] = sourceReg;
		args[1] = addrReg;
	}

	void BuildStore32Reg( int sourceReg, int addrReg ) {
		op = ST_32_REG;
		args[0] = sourceReg;
		args[1] = addrReg;
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
	void BuildAndImm32( int sourceReg, int destReg, uint32_t imm ) {
		op = AND_IMM_32;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = imm;
	}

	void BuildAndImm64( int sourceReg, int destReg, uint64_t imm ) {
		op = AND_IMM_64;
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

	void BuildAndcImm( int sourceReg0, int sourceReg1, uint64_t imm ) {
		op = ANDC_IMM;
		args[0] = sourceReg0;
		args[1] = sourceReg1;
		args[2] = imm;
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

	void BuildXorImm( int sourceReg, int destReg, uint64_t imm ) {
		op = XOR_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = imm;
	}

//Shift/Rotate
	void BuildRol32Imm( int sourceReg, int destReg, int rotate ) {
		op = ROL32_IMM;
		args[0] = sourceReg;
		args[1] = destReg;
		args[2] = rotate;
	}

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

	void BuildSlr32Imm( int sourceReg, int destReg, int shift ) {
		op = SLR32_IMM;
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

	void BuildByteSwap32( int sourceReg, int destReg ) {
		op = BYTE_SWAP_32;
		args[0] = sourceReg;
		args[1] = destReg;
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

	void BuildPpcCmpd( int ra, int rb, int cr ) {
		op = PPC_CMPD;
		args[0] = ra;
		args[1] = rb;
		args[2] = cr;
	}

	void BuildPpcCmpdi( int reg, int cr, int32_t imm ) {
		op = PPC_CMPDI;
		args[0] = reg;
		args[1] = cr;
		args[2] = imm;
	}

	void BuildPpcCmpw( int ra, int rb, int cr ) {
		op = PPC_CMPW;
		args[0] = ra;
		args[1] = rb;
		args[2] = cr;
	}

	void BuildPpcCmpwi( int reg, int cr, int32_t imm ) {
		op = PPC_CMPWI;
		args[0] = reg;
		args[1] = cr;
		args[2] = imm;
	}

	void BuildPpcCmpld( int ra, int rb, int cr ) {
		op = PPC_CMPLD;
		args[0] = ra;
		args[1] = rb;
		args[2] = cr;
	}

	void BuildPpcCmpldi( int reg, int cr, uint32_t imm ) {
		op = PPC_CMPLDI;
		args[0] = reg;
		args[1] = cr;
		args[2] = imm;
	}

	void BuildPpcCmplw( int ra, int rb, int cr ) {
		op = PPC_CMPLW;
		args[0] = ra;
		args[1] = rb;
		args[2] = cr;
	}

	void BuildPpcCmplwi( int reg, int cr, uint32_t imm ) {
		op = PPC_CMPLWI;
		args[0] = reg;
		args[1] = cr;
		args[2] = imm;
	}

	void BuildPpcRfid() {
		op = PPC_RFID;
	}

	void BuildPpcSrawi( int rs, int ra, int sh, bool rc ) {
		op = PPC_SRAWI;
		args[0] = rs;
		args[1] = ra;
		args[2] = sh;
		args[3] = rc;
	}

	void BuildPpcSubfic( int rs, int ra, int64_t imm ) {
		op = PPC_SUBFIC;
		args[0] = rs;
		args[1] = ra;
		args[2] = imm;
	}
};

} //namespace jit

#endif //GEN7_LIBJIT_INTERINSTR_H

