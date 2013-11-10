#include "jit/CpuInterpreter.h"
#include "jit/InterInstr.h"

#include <gtest/gtest.h>

using namespace jit;

class TestCpuInterpreter : public CpuInterpreter
{
public:
	static const int NUM_GPRS = 32;
	static const int NUM_SYS_REGS = 1024;

	uint64_t pc;
	uint64_t gprs[ NUM_GPRS ];
	uint64_t sysRegs[ NUM_SYS_REGS ];

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
	}

	TestCpuInterpreter()
	  : CpuInterpreter( &gprs[0] )
	{
		Reset();
	}
};

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

	instr.BuildSetSystemImm( 0xFFFFFFFF00000000UL, 512 );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.sysRegs[512] );

	//Test invalid system reg
	testCpu.Reset();
	instr.BuildSetSystemImm( 0xFFFFFFFF00000000UL, TestCpuInterpreter::NUM_SYS_REGS );

	EXPECT_FALSE( testCpu.InterpretIntermediate( instr ) );
}

TEST(CpuInterpreter, SetSystemReg)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	instr.BuildSetSystemReg( testCpu.Gpr64Offset(1), 512 );

	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.sysRegs[512] );

	//Test invalid system reg
	testCpu.Reset();
	instr.BuildSetSystemReg( 21, TestCpuInterpreter::NUM_SYS_REGS );

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

TEST(CpuInterpreter, MoveReg)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	instr.BuildMoveReg( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2) );

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

TEST(CpuInterpreter, BranchGprNotZero)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 1;
	instr.BuildBranchGprNotZero( testCpu.Gpr64Offset(1), 0xFFFFFFFF00000000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.pc );

	testCpu.Reset();

	testCpu.gprs[1] = 0;
	instr.BuildBranchGprNotZero( testCpu.Gpr64Offset(1), 0xFFFFFFFF00000000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0, testCpu.pc );
}

TEST(CpuInterpreter, LoadImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	instr.BuildLoadImm( testCpu.Gpr64Offset(1), 0xFFFFFFFF00000000UL );
	EXPECT_TRUE( testCpu.InterpretIntermediate( instr ) );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, testCpu.gprs[1] );
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

TEST(CpuInterpreter, AndImm)
{
	TestCpuInterpreter testCpu;
	InterInstr instr;

	testCpu.gprs[1] = 0xFFFFFFFF00000000UL;
	instr.BuildAndImm( testCpu.Gpr64Offset(1), testCpu.Gpr64Offset(2), 
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

