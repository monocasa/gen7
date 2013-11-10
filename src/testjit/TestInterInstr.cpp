#include "jit/InterInstr.h"

#include <gtest/gtest.h>

using namespace jit;

TEST(InterInstr, Unknown)
{
	InterInstr instr;

	instr.BuildUnknown( 5, 65, 1 );

	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr.op );
	EXPECT_EQ( 5, instr.args[0] );
	EXPECT_EQ( 65, instr.args[1] );
	EXPECT_EQ( 1, instr.args[2] );
}

TEST(InterInstr, Nop)
{
	InterInstr instr;

	instr.BuildNop();

	EXPECT_EQ( InstrOp::NOP, instr.op );
}

TEST(InterInstr, SetSystemImm)
{
	InterInstr instr;

	instr.BuildSetSystemImm( 0xFFFFFFFF00000000UL, 512 );

	EXPECT_EQ( InstrOp::SET_SYS_IMM, instr.op );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[0] );
	EXPECT_EQ( 512, instr.args[1] );
}

TEST(InterInstr, SetSystemReg)
{
	InterInstr instr;

	instr.BuildSetSystemReg( 31, 512 );

	EXPECT_EQ( InstrOp::SET_SYS_REG, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 512, instr.args[1] );
}

TEST(InterInstr, ReadSystem)
{
	InterInstr instr;

	instr.BuildReadSystem( 31, 512 );

	EXPECT_EQ( InstrOp::READ_SYS, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 512, instr.args[1] );
}

TEST(InterInstr, MoveReg)
{
	InterInstr instr;

	instr.BuildMoveReg( 20, 30 );

	EXPECT_EQ( InstrOp::MOVE_REG, instr.op );
	EXPECT_EQ( 20, instr.args[0] );
	EXPECT_EQ( 30, instr.args[1] );
}

TEST(InterInstr, BranchAlways)
{
	InterInstr instr;

	instr.BuildBranchAlways( 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr.op );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[0] );
}

TEST(InterInstr, BranchGprNotZero)
{
	InterInstr instr;

	instr.BuildBranchGprNotZero( 31, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::BRANCH_GPR_NOT_ZERO, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstr, LoadImm)
{
	InterInstr instr;

	instr.BuildLoadImm( 31, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::LOAD_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstr, Add)
{
	InterInstr instr;

	instr.BuildAdd( 31, 1, 2 );

	EXPECT_EQ( InstrOp::ADD, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 2, instr.args[2] );
}

TEST(InterInstr, AddImm)
{
	InterInstr instr;

	instr.BuildAddImm( 31, 1, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::ADD_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstr, Sub)
{
	InterInstr instr;

	instr.BuildSub( 31, 1, 2 );

	EXPECT_EQ( InstrOp::SUB, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 2, instr.args[2] );
}

TEST(InterInstr, SubuImm)
{
	InterInstr instr;

	instr.BuildSubuImm( 31, 1, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::SUBU_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstr, AndImm)
{
	InterInstr instr;

	instr.BuildAndImm( 31, 1, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::AND_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstr, Andc)
{
	InterInstr instr;

	instr.BuildAndc( 31, 1, 2 );

	EXPECT_EQ( InstrOp::ANDC, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 2, instr.args[2] );
}

TEST(InterInstr, Or)
{
	InterInstr instr;

	instr.BuildOr( 31, 1, 2 );

	EXPECT_EQ( InstrOp::OR, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 2, instr.args[2] );
}

TEST(InterInstr, OrImm)
{
	InterInstr instr;

	instr.BuildOrImm( 31, 1, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::OR_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstr, Sll32Imm)
{
	InterInstr instr;

	instr.BuildSll32Imm( 31, 1, 2 );

	EXPECT_EQ( InstrOp::SLL32_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 2, instr.args[2] );
}

TEST(InterInstr, Sll64Imm)
{
	InterInstr instr;

	instr.BuildSll64Imm( 31, 1, 2 );

	EXPECT_EQ( InstrOp::SLL64_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 2, instr.args[2] );
}

TEST(InterInstr, PpcSlbia)
{
	InterInstr instr;

	instr.BuildPpcSlbia();

	EXPECT_EQ( InstrOp::PPC_SLBIA, instr.op );
}

TEST(InterInstr, PpcSlbmte)
{
	InterInstr instr;

	instr.BuildPpcSlbmte( 2, 4 );

	EXPECT_EQ( InstrOp::PPC_SLBMTE, instr.op );
	EXPECT_EQ( 2, instr.args[0] );
	EXPECT_EQ( 4, instr.args[1] );
}

TEST(InterInstr, PpcTlbiel)
{
	InterInstr instr;

	instr.BuildPpcTlbiel( 4, true );

	EXPECT_EQ( InstrOp::PPC_TLBIEL, instr.op );
	EXPECT_EQ( 4, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
}

