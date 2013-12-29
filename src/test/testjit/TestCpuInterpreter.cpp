#include "jit/CpuInterpreter.h"
#include "jit/InterInstr.h"

#include <gtest/gtest.h>

#include <map>
#include <vector>

using namespace jit;

class TestMemoryPolicy
{
public:
	struct Access {
		uint64_t addr;
		uint64_t value;
		enum Type : int {
			READ32,
			READ64,
			WRITE8,
			WRITE32,
			WRITE64,
		} type;

		Access( Type type, uint64_t addr, uint64_t value )
		  : addr( addr )
		  , value( value )
		  , type( type )
		{ }
	};

protected:
	uint32_t ReadMem32( uint64_t addr ) {
		uint32_t value = 0;

		if( data32.end() != data32.find(addr) ) {
			value = data32[ addr ];
		}

		accesses.push_back( Access(Access::Type::READ32, addr, value) );

		return value;
	}

	uint64_t ReadMem64( uint64_t addr ) {
		uint64_t value = 0;

		if( data64.end() != data64.find(addr) ) {
			value = data64[ addr ];
		}

		accesses.push_back( Access(Access::Type::READ64, addr, value) );

		return value;
	}

	void WriteMem8( uint64_t addr, uint8_t value ) {
		data8[ addr ] = value;

		accesses.push_back( Access(Access::Type::WRITE8, addr, value ) );
	}

	void WriteMem32( uint64_t addr, uint32_t value ) {
		data32[ addr ] = value;

		accesses.push_back( Access(Access::Type::WRITE32, addr, value) );
	}

	void WriteMem64( uint64_t addr, uint64_t value ) {
		data64[ addr ] = value;

		accesses.push_back( Access(Access::Type::WRITE64, addr, value) );
	}

public:
	std::map<uint64_t, uint8_t>  data8;
	std::map<uint64_t, uint32_t> data32;
	std::map<uint64_t, uint64_t> data64;
	std::vector<Access> accesses;

	void ResetMemoryPolicy() {
		data8.clear();
		data32.clear();
		data64.clear();
		accesses.clear();
	}
};

class TestCpuInterpreter : public CpuInterpreter<TestMemoryPolicy>
{
public:
	static const int NUM_GPRS = 32;
	static const int NUM_SYS_REGS = 1024;

	uint64_t pc;
	uint64_t gprs[ NUM_GPRS ];
	uint64_t sysRegs[ NUM_SYS_REGS ];

	bool isReserved;
	uint64_t reservation;

	virtual void SetPC( uint64_t newPc ) {
		pc = newPc;
	}

	virtual bool SetSystemReg( int sysRegNum, uint64_t value ) {
		if( sysRegNum < NUM_SYS_REGS ) {
			sysRegs[ sysRegNum ] = value;
			return true;
		}
		return false;
	}

	virtual bool ReadSystemReg( int sysRegNum, uint64_t &value ) {
		if( sysRegNum < NUM_SYS_REGS ) {
			value = sysRegs[ sysRegNum ];
			return true;
		}
		return false;
	}

	virtual bool InterpretProcessorSpecific( jit::InterInstr &/*instr*/ ) {
		return true;
	}

	int Gpr64Offset( int reg ) {
		return reg * sizeof(uint64_t);
	}

	int Gpr32OffsetLow( int reg ) {
		return reg * sizeof(uint64_t);
	}

	int Gpr32OffsetHigh( int reg ) {
		return (reg * sizeof(uint64_t)) + sizeof(uint32_t);
	}

	void Reset() {
		for( auto &gpr : gprs ) {
			gpr = 0;
		}

		for( auto &sysReg : sysRegs ) {
			sysReg = 0;
		}

		pc = 0;
		isReserved = false;
		reservation = 0;

		ResetMemoryPolicy();
	}

	bool IsReserved() const {
		return isReserved;
	}

	uint64_t Reservation() const {
		return reservation;
	}

	TestCpuInterpreter()
	  : CpuInterpreter( &gprs[0], isReserved, reservation )
	{
		Reset();
	}
};

TEST(CpuInterpreter, ReadRegGPR32)
{
 	TestCpuInterpreter testCpu;
	Operand<int> sourceReg;
	uint64_t value;

	testCpu.gprs[1] = 0xFFFFFFFFFFFFFFFFUL;
	sourceReg.Set<OpType::GPR32>( testCpu.Gpr64Offset(1) );
	EXPECT_TRUE( testCpu.ReadReg(sourceReg, value) );
	EXPECT_EQ( 0x00000000FFFFFFFFUL, value );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.gprs[1] );
}

TEST(CpuInterpreter, ReadRegGPR64)
{
	TestCpuInterpreter testCpu;
	Operand<int> sourceReg;
	uint64_t value;

	testCpu.gprs[1] = 0xFFFFFFFFFFFFFFFFUL;
	sourceReg.Set<OpType::GPR64>( testCpu.Gpr64Offset(1) );
	EXPECT_TRUE( testCpu.ReadReg(sourceReg, value) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, value );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.gprs[1] );
}

TEST(CpuInterpreter, ReadRegSYS32)
{
 	TestCpuInterpreter testCpu;
	Operand<int> sourceReg;
	uint64_t value;

	testCpu.sysRegs[1] = 0xFFFFFFFFFFFFFFFFUL;
	sourceReg.Set<OpType::SYS32>( 1 );
	EXPECT_FALSE( testCpu.ReadReg(sourceReg, value) );
}

TEST(CpuInterpreter, ReadRegSYS64)
{
 	TestCpuInterpreter testCpu;
	Operand<int> sourceReg;
	uint64_t value;

	testCpu.sysRegs[1] = 0xFFFFFFFFFFFFFFFFUL;
	sourceReg.Set<OpType::SYS64>( 1 );
	EXPECT_TRUE( testCpu.ReadReg(sourceReg, value) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, value );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.sysRegs[1] );

	sourceReg.Set<OpType::SYS64>( TestCpuInterpreter::NUM_SYS_REGS );
	EXPECT_FALSE( testCpu.ReadReg(sourceReg, value) );
}

TEST(CpuInterpreter, SetRegGPR32)
{
	TestCpuInterpreter testCpu;
	Operand<int> destReg;
	const uint64_t value = 0xFFFFFFFFFFFFFFFFUL;

	destReg.Set<OpType::GPR32>( testCpu.Gpr64Offset(1) );
	EXPECT_TRUE( testCpu.SetReg(destReg, value) );
	EXPECT_EQ( 0x00000000FFFFFFFFUL, testCpu.gprs[1] );
}

TEST(CpuInterpreter, SetRegGPR64)
{
	TestCpuInterpreter testCpu;
	Operand<int> destReg;
	const uint64_t value = 0xFFFFFFFFFFFFFFFFUL;

	destReg.Set<OpType::GPR64>( testCpu.Gpr64Offset(1) );
	EXPECT_TRUE( testCpu.SetReg(destReg, value) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.gprs[1] );
}

TEST(CpuInterpreter, SetRegSYS32)
{
	TestCpuInterpreter testCpu;
	Operand<int> destReg;
	const uint64_t value = 0xFFFFFFFFFFFFFFFFUL;

	destReg.Set<OpType::SYS32>( 1 );
	EXPECT_FALSE( testCpu.SetReg(destReg, value) );
}

TEST(CpuInterpreter, SetRegSYS64)
{
	TestCpuInterpreter testCpu;
	Operand<int> destReg;
	const uint64_t value = 0xFFFFFFFFFFFFFFFFUL;

	destReg.Set<OpType::SYS64>( 1 );
	EXPECT_TRUE( testCpu.SetReg(destReg, value) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.sysRegs[1] );
}

TEST(CpuInterpreter, Unknown)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildUnknown( 5, 0x41, 1 );

	EXPECT_FALSE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_STREQ( "Unknown opcode 5 (00000041) @ 0000000000000001", testCpu.GetErrorString() );
}

TEST(CpuInterpreter, Nop)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildNop();

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );

	for( auto gpr : testCpu.gprs ) {
		EXPECT_EQ( 0, gpr );
	}

	for( auto sysReg : testCpu.sysRegs ) {
		EXPECT_EQ( 0, sysReg );
	}

	EXPECT_EQ( 0, testCpu.pc );
}

TEST(CpuInterpreter, SetSystemImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildSetSystemImm( 512, 0xFFFFFFFF00000000UL );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.sysRegs[512] );

	//Test invalid system reg
	testCpu.Reset();
	instr.BuildSetSystemImm( TestCpuInterpreter::NUM_SYS_REGS, 0xFFFFFFFF00000000UL );

	EXPECT_FALSE( testCpu.InterpretIntermediate( instr ) );
}

TEST(CpuInterpreter, SetSystemReg)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	instr.BuildSetSystemReg( 512, testCpu.Gpr64Offset(1) );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.sysRegs[512] );

	//Test invalid system reg
	testCpu.Reset();
	instr.BuildSetSystemReg( TestCpuInterpreter::NUM_SYS_REGS, 21 );

	EXPECT_FALSE( testCpu.InterpretIntermediate( instr ) );
}

TEST(CpuInterpreter, ReadSystem)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.sysRegs[512] = 0xFFFFFFFF00000000UL;
	instr.BuildReadSystem( testCpu.Gpr64Offset(1), 512 );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.sysRegs[512] );

	//Test invalid system reg
	testCpu.Reset();
	instr.BuildReadSystem( 21, TestCpuInterpreter::NUM_SYS_REGS );

	EXPECT_FALSE( testCpu.InterpretIntermediate( instr ) );
}

TEST(CpuInterpreter, MoveReg32)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xBBBBBBBB55555555UL;
	testCpu.gprs[2] = 0xAAAAAAAA00000000UL;
	instr.BuildMoveReg32( testCpu.Gpr32OffsetLow(2), testCpu.Gpr32OffsetLow(1) );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xBBBBBBBB55555555UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xAAAAAAAA55555555UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, MoveReg64)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	instr.BuildMoveReg64( testCpu.Gpr64Offset(2), testCpu.Gpr64Offset(1) );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, BranchAlways)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildBranchAlways( 0xFFFFFFFF00000000UL );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.pc );
}

TEST(CpuInterpreter, BranchGpr64)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;

	instr.BuildBranchGpr64( testCpu.Gpr64Offset(1) );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.pc );
}

TEST(CpuInterpreter, BranchGpr32MaskZero)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildBranchGpr32MaskZero( testCpu.Gpr32OffsetLow(1), 0x80, 0xFFFFFFFF00000000UL );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.pc );

	testCpu.Reset();
	testCpu.gprs[1] = 0x80;

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0, testCpu.pc );
}

TEST(CpuInterpreter, BranchGpr32MaskNotZero)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildBranchGpr32MaskNotZero( testCpu.Gpr32OffsetLow(1), 0x80, 0xFFFFFFFF00000000UL );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0, testCpu.pc );

	testCpu.Reset();
	testCpu.gprs[1] = 0x80;

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.pc );
}

TEST(CpuInterpreter, BranchGpr64NotZero)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 1;
	instr.BuildBranchGpr64NotZero( testCpu.Gpr64Offset(1), 0xFFFFFFFF00000000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.pc );

	testCpu.Reset();

	testCpu.gprs[1] = 0;
	instr.BuildBranchGpr64NotZero( testCpu.Gpr64Offset(1), 0xFFFFFFFF00000000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0, testCpu.pc );
}

TEST(CpuInterpreter, BranchGpr64NotZeroGpr64)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 1;
	testCpu.gprs[2] = 0xFFFFFFFF00000000UL;
	instr.BuildBranchGpr64NotZeroGpr64( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.pc );

	testCpu.Reset();

	testCpu.gprs[1] = 0;
	testCpu.gprs[2] = 0xFFFFFFFF00000000UL;
	instr.BuildBranchGpr64NotZeroGpr64( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0, testCpu.pc );
}

TEST(CpuInterpreter, Load32Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildLoad32Imm( testCpu.Gpr32OffsetLow(1), 0xFFFF0000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x00000000FFFF0000, testCpu.gprs[1] );

	testCpu.Reset();

	instr.BuildLoad32Imm( testCpu.Gpr32OffsetHigh(1), 0xFFFF0000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFF000000000000, testCpu.gprs[1] );
}

TEST(CpuInterpreter, Load32)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	testCpu.data32[ 0x100 ] = 0x55555555;

	instr.BuildLoad32Abs( testCpu.Gpr32OffsetLow(1), 0x0000000000000100UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF55555555UL, testCpu.gprs[1] );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::READ32, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000000100UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x0000000055555555UL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Load32RegOffset)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	testCpu.gprs[2] = 0x0000000000001000UL;
	testCpu.data32[ 0x1100 ] = 0x55555555;

	instr.BuildLoad32RegOffset( testCpu.Gpr32OffsetLow(1), testCpu.Gpr64Offset(2), 0x0000000000000100UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF55555555UL, testCpu.gprs[1] );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::READ32, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000001100UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x0000000055555555UL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Load64RegOffset)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	testCpu.gprs[2] = 0x0000000000001000UL;
	testCpu.data64[ 0x1100 ] = 0x5555555555555555UL;

	instr.BuildLoad64RegOffset( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2), 0x0000000000000100UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x5555555555555555UL, testCpu.gprs[1] );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::READ64, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000001100UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x5555555555555555UL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Load32Linked)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0x0000000000000100UL;
	testCpu.gprs[2] = 0xFFFFFFFF00000000UL;
	testCpu.data32[ 0x100 ] = 0x55555555;

	instr.BuildLoad32Linked( testCpu.Gpr64Offset(2), testCpu.Gpr32OffsetLow(1) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x0000000000000100UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFF55555555UL, testCpu.gprs[2] );
	EXPECT_TRUE( testCpu.IsReserved() );
	EXPECT_EQ( 0x0000000000000100UL, testCpu.Reservation() );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::READ32, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000000100UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x0000000055555555UL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Load64Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildLoad64Imm( testCpu.Gpr64Offset(1), 0xFFFFFFFF00000000UL );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
}

TEST(CpuInterpreter, Store64)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0x0123456789ABCDEFUL;

	instr.BuildStore64( testCpu.Gpr64Offset(1), 0x0000000000001100UL );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x0123456789ABCDEFUL, testCpu.gprs[1] );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::WRITE64, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000001100UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x0123456789ABCDEFUL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Store8RegOffset)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0x0123456789ABCDEFUL;
	testCpu.gprs[2] = 0x0000000000001000UL;

	instr.BuildStore8Reg( testCpu.Gpr32OffsetLow(1), testCpu.Gpr64Offset(2) );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x0123456789ABCDEFUL, testCpu.gprs[1] );
	EXPECT_EQ( 0x0000000000001000UL, testCpu.gprs[2] );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::WRITE8, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000001000UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x000000EFUL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Store32RegOffset)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0x0123456789ABCDEFUL;
	testCpu.gprs[2] = 0x0000000000001000UL;

	instr.BuildStore32Reg( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2) );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x0123456789ABCDEFUL, testCpu.gprs[1] );
	EXPECT_EQ( 0x0000000000001000UL, testCpu.gprs[2] );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::WRITE32, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000001000UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x89ABCDEFUL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Store64RegOffset)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0x0123456789ABCDEFUL;
	testCpu.gprs[2] = 0x0000000000001000UL;

	instr.BuildStore64RegOffset( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2), 0x0000000000000100 );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x0123456789ABCDEFUL, testCpu.gprs[1] );
	EXPECT_EQ( 0x0000000000001000UL, testCpu.gprs[2] );

	ASSERT_EQ( 1, testCpu.accesses.size() );
	EXPECT_EQ( TestCpuInterpreter::Access::Type::WRITE64, testCpu.accesses[0].type );
	EXPECT_EQ( 0x0000000000001100UL, testCpu.accesses[0].addr );
	EXPECT_EQ( 0x0123456789ABCDEFUL, testCpu.accesses[0].value );
}

TEST(CpuInterpreter, Add)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 1;
	testCpu.gprs[2] = 2;
	instr.BuildAdd( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2),
	                testCpu.Gpr64Offset(3) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 1, testCpu.gprs[1] );
	EXPECT_EQ( 2, testCpu.gprs[2] );
	EXPECT_EQ( 3, testCpu.gprs[3] );
}

TEST(CpuInterpreter, AddImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 1;
	instr.BuildAddImm( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(3),
	                2 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 1, testCpu.gprs[1] );
	EXPECT_EQ( 0, testCpu.gprs[2] );
	EXPECT_EQ( 3, testCpu.gprs[3] );
}

TEST(CpuInterpreter, Sub)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 1;
	testCpu.gprs[2] = 2;
	instr.BuildSub( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2),
	                testCpu.Gpr64Offset(3) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 1, testCpu.gprs[1] );
	EXPECT_EQ( 2, testCpu.gprs[2] );
	EXPECT_EQ( 1, testCpu.gprs[3] );
}

TEST(CpuInterpreter, SubuImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 1;
	instr.BuildSubuImm( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(3), 2 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 1, testCpu.gprs[1] );
	EXPECT_EQ( 0, testCpu.gprs[2] );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.gprs[3] );
}

TEST(CpuInterpreter, AndImm32)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFF0000UL;
	instr.BuildAndImm32( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2), 
	                     0x00FFFF00UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFF0000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0x00FF0000UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, AndImm64)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	instr.BuildAndImm64( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2), 
	                     0x0000FFFFFFFF0000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0x0000FFFF00000000UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Andc)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFFFFFFFFFFUL;
	testCpu.gprs[2] = 0x000000000000000FUL;
	instr.BuildAndc( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2),
	                 testCpu.Gpr64Offset(3) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.gprs[1] );
	EXPECT_EQ( 0x000000000000000FUL, testCpu.gprs[2] );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFF0UL, testCpu.gprs[3] );
}

TEST(CpuInterpreter, AndcImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	instr.BuildAndcImm( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2), 
	                    0x0000FFFFFFFF0000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFF000000000000UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Or)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFFFFFFFFF0UL;
	testCpu.gprs[2] = 0x000000000000000FUL;
	instr.BuildOr( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2),
	               testCpu.Gpr64Offset(3) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFF0UL, testCpu.gprs[1] );
	EXPECT_EQ( 0x000000000000000FUL, testCpu.gprs[2] );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.gprs[3] );
}

TEST(CpuInterpreter, OrImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFFFFFFFFF0UL;
	instr.BuildOrImm( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2),
	                  0x000000000000000FUL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFF0UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, XorImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFFFFFFFFF0UL;
	instr.BuildXorImm( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2),
	                   0x00000000000000FFUL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFF0UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFFFFFFFF0FUL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Rol32Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xAAAAAAAAFFF00001UL;
	testCpu.gprs[2] = 0xBBBBBBBB00000000UL;
	instr.BuildRol32Imm( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2),
	                     16 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xAAAAAAAAFFF00001UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xBBBBBBBB0001FFF0UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Rol64Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000001UL;
	instr.BuildRol64Imm( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2),
	                     16 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000001UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFF00000001FFFFUL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Sll32Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000001UL;
	testCpu.gprs[2] = 0xFFFFFFFF00000000UL;
	instr.BuildSll32Imm( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2),
	                     16 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000001UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFF00010000UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Sll64Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000001UL;
	instr.BuildSll64Imm( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2),
	                     16 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000001UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFF000000010000UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Slr32Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xAAAAAAAA00010000UL;
	testCpu.gprs[2] = 0xBBBBBBBB00000000UL;
	instr.BuildSlr32Imm( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2),
	                     16 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xAAAAAAAA00010000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xBBBBBBBB00000001UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, Slr64Imm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000001UL;
	instr.BuildSlr64Imm( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2),
	                     16 );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000001UL, testCpu.gprs[1] );
	EXPECT_EQ( 0x0000FFFFFFFF0000UL, testCpu.gprs[2] );
}

TEST(CpuInterpreter, ByteSwap32)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0x0123456789abcdefUL;
	instr.BuildByteSwap32( testCpu.Gpr32OffsetLow(1), testCpu.Gpr32OffsetLow(2) );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0x0123456789abcdefUL, testCpu.gprs[1] );
	EXPECT_EQ( 0x00000000efcdab89UL, testCpu.gprs[2] );
}

