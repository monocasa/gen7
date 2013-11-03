#include "jit/InterInstr.h"

#include <gtest/gtest.h>

using namespace jit;

TEST(InterInstrTest, Unknown)
{
	InterInstr instr;

	instr.BuildUnknown( 5, 65, 1 );

	ASSERT_EQ( InstrOp::UNKNOWN_OPCODE, instr.op );
	ASSERT_EQ( 5, instr.args[0] );
	ASSERT_EQ( 65, instr.args[1] );
	ASSERT_EQ( 1, instr.args[2] );
}

TEST(InterInstrTest, Nop)
{
	InterInstr instr;

	instr.BuildNop();

	ASSERT_EQ( InstrOp::NOP, instr.op );
}

TEST(InterInstrTest, SetSystemImm)
{
	InterInstr instr;

	instr.BuildSetSystemImm( 0xFFFFFFFF00000000UL, 512 );

	ASSERT_EQ( InstrOp::SET_SYS_IMM, instr.op );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[0] );
	ASSERT_EQ( 512, instr.args[1] );
}

TEST(InterInstrTest, SetSystemReg)
{
	InterInstr instr;

	instr.BuildSetSystemReg( 31, 512 );

	ASSERT_EQ( InstrOp::SET_SYS_REG, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 512, instr.args[1] );
}

TEST(InterInstrTest, ReadSystem)
{
	InterInstr instr;

	instr.BuildReadSystem( 31, 512 );

	ASSERT_EQ( InstrOp::READ_SYS, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 512, instr.args[1] );
}

TEST(InterInstrTest, MoveReg)
{
	InterInstr instr;

	instr.BuildMoveReg( 20, 30 );

	ASSERT_EQ( InstrOp::MOVE_REG, instr.op );
	ASSERT_EQ( 20, instr.args[0] );
	ASSERT_EQ( 30, instr.args[1] );
}

TEST(InterInstrTest, BranchAlways)
{
	InterInstr instr;

	instr.BuildBranchAlways( 0xFFFFFFFF00000000UL );

	ASSERT_EQ( InstrOp::BRANCH_ALWAYS, instr.op );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[0] );
}

TEST(InterInstrTest, BranchGprNotZero)
{
	InterInstr instr;

	instr.BuildBranchGprNotZero( 31, 0xFFFFFFFF00000000UL );

	ASSERT_EQ( InstrOp::BRANCH_GPR_NOT_ZERO, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstrTest, LoadImm)
{
	InterInstr instr;

	instr.BuildLoadImm( 31, 0xFFFFFFFF00000000UL );

	ASSERT_EQ( InstrOp::LOAD_IMM, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstrTest, Add)
{
	InterInstr instr;

	instr.BuildAdd( 31, 1, 2 );

	ASSERT_EQ( InstrOp::ADD, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 2, instr.args[2] );
}

TEST(InterInstrTest, AddImm)
{
	InterInstr instr;

	instr.BuildAddImm( 31, 1, 0xFFFFFFFF00000000UL );

	ASSERT_EQ( InstrOp::ADD_IMM, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstrTest, Sub)
{
	InterInstr instr;

	instr.BuildSub( 31, 1, 2 );

	ASSERT_EQ( InstrOp::SUB, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 2, instr.args[2] );
}

TEST(InterInstrTest, SubuImm)
{
	InterInstr instr;

	instr.BuildSubuImm( 31, 1, 0xFFFFFFFF00000000UL );

	ASSERT_EQ( InstrOp::SUBU_IMM, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstrTest, AndImm)
{
	InterInstr instr;

	instr.BuildAndImm( 31, 1, 0xFFFFFFFF00000000UL );

	ASSERT_EQ( InstrOp::AND_IMM, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstrTest, Andc)
{
	InterInstr instr;

	instr.BuildAndc( 31, 1, 2 );

	ASSERT_EQ( InstrOp::ANDC, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 2, instr.args[2] );
}

TEST(InterInstrTest, Or)
{
	InterInstr instr;

	instr.BuildOr( 31, 1, 2 );

	ASSERT_EQ( InstrOp::OR, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 2, instr.args[2] );
}

TEST(InterInstrTest, OrImm)
{
	InterInstr instr;

	instr.BuildOrImm( 31, 1, 0xFFFFFFFF00000000UL );

	ASSERT_EQ( InstrOp::OR_IMM, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstrTest, Sll32Imm)
{
	InterInstr instr;

	instr.BuildSll32Imm( 31, 1, 2 );

	ASSERT_EQ( InstrOp::SLL32_IMM, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 2, instr.args[2] );
}

TEST(InterInstrTest, Sll64Imm)
{
	InterInstr instr;

	instr.BuildSll64Imm( 31, 1, 2 );

	ASSERT_EQ( InstrOp::SLL64_IMM, instr.op );
	ASSERT_EQ( 31, instr.args[0] );
	ASSERT_EQ( 1, instr.args[1] );
	ASSERT_EQ( 2, instr.args[2] );
}

TEST(InterInstrTest, PpcSlbia)
{
	InterInstr instr;

	instr.BuildPpcSlbia();

	ASSERT_EQ( InstrOp::PPC_SLBIA, instr.op );
}

TEST(InterInstrTest, PpcSlbmte)
{
	InterInstr instr;

	instr.BuildPpcSlbmte( 2, 4 );

	ASSERT_EQ( InstrOp::PPC_SLBMTE, instr.op );
	ASSERT_EQ( 2, instr.args[0] );
	ASSERT_EQ( 4, instr.args[1] );
}

TEST(InterInstrTest, PpcTlbiel)
{
	InterInstr instr;

	instr.BuildPpcTlbiel( 4 );

	ASSERT_EQ( InstrOp::PPC_TLBIEL, instr.op );
	ASSERT_EQ( 4, instr.args[0] );
}

