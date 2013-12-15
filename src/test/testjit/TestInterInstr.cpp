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

TEST(InterInstr, Invalid)
{
	InterInstr instr;

	instr.BuildInvalid( 5, 65, 1 );

	EXPECT_EQ( InstrOp::INVALID_OPCODE, instr.op );
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

TEST(InterInstr, MoveReg32)
{
	InterInstr instr;

	instr.BuildMoveReg32( 20, 30 );

	EXPECT_EQ( InstrOp::MOVE_REG32, instr.op );
	EXPECT_EQ( 20, instr.args[0] );
	EXPECT_EQ( 30, instr.args[1] );
}

TEST(InterInstr, MoveReg64)
{
	InterInstr instr;

	instr.BuildMoveReg64( 20, 30 );

	EXPECT_EQ( InstrOp::MOVE_REG64, instr.op );
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

TEST(InterInstr, BranchGpr64)
{
	InterInstr instr;

	instr.BuildBranchGpr64( 31 );

	EXPECT_EQ( InstrOp::BRANCH_GPR64, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
}

TEST(InterInstr, BranchGpr32MaskZero)
{
	InterInstr instr;

	instr.BuildBranchGpr32MaskZero( 31, 0x01234567, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::BRANCH_GPR32_MASK_ZERO, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 0x01234567, instr.args[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstr, BranchGpr32MaskNotZero)
{
	InterInstr instr;

	instr.BuildBranchGpr32MaskNotZero( 31, 0x01234567, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::BRANCH_GPR32_MASK_NOT_ZERO, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 0x01234567, instr.args[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
}

TEST(InterInstr, BranchGpr64NotZero)
{
	InterInstr instr;

	instr.BuildBranchGpr64NotZero( 31, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::BRANCH_GPR64_NOT_ZERO, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstr, Load32Imm)
{
	InterInstr instr;

	instr.BuildLoad32Imm( 31, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::LD_32_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstr, Load64Imm)
{
	InterInstr instr;

	instr.BuildLoad64Imm( 31, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::LD_64_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstr, Load32)
{
	InterInstr instr;

	instr.BuildLoad32( 1, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::LD_32, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[1] );
}

TEST(InterInstr, Load32RegOffset)
{
	InterInstr instr;

	instr.BuildLoad32RegOffset( 1, 2, -1 );

	EXPECT_EQ( InstrOp::LD_32_REG_OFF, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( -1, instr.args[2] );
}

TEST(InterInstr, Load64RegOffset)
{
	InterInstr instr;

	instr.BuildLoad64RegOffset( 1, 2, -1 );

	EXPECT_EQ( InstrOp::LD_64_REG_OFF, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( -1, instr.args[2] );
}

TEST(InterInstr, Load32Linked)
{
	InterInstr instr;

	instr.BuildLoad32Linked( 1, 2 );

	EXPECT_EQ( InstrOp::LD_32_L, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
}

TEST(InterInstr, Load32IndexedLinked)
{
	InterInstr instr;

	instr.BuildLoad32IndexedLinked( 1, 2, 3 );

	EXPECT_EQ( InstrOp::LD_32_IDX_L, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, Store32)
{
	InterInstr instr;

	instr.BuildStore32( 1, 2 );

	EXPECT_EQ( InstrOp::ST_32, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
}

TEST(InterInstr, Store64)
{
	InterInstr instr;

	instr.BuildStore64( 1, 2 );

	EXPECT_EQ( InstrOp::ST_64, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
}

TEST(InterInstr, Store32Reg)
{
	InterInstr instr;

	instr.BuildStore32Reg( 1, 2 );

	EXPECT_EQ( InstrOp::ST_32_REG, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
}

TEST(InterInstr, Store64RegOffset)
{
	InterInstr instr;

	instr.BuildStore64RegOffset( 1, 2, -1 );

	EXPECT_EQ( InstrOp::ST_64_REG_OFF, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( -1, instr.args[2] );
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

TEST(InterInstr, AndcImm)
{
	InterInstr instr;

	instr.BuildAndcImm( 31, 1, 0xFFFFFFFF00000000UL );

	EXPECT_EQ( InstrOp::ANDC_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 0xFFFFFFFF00000000UL, instr.args[2] );
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

TEST(InterInstr, Rol32Imm)
{
	InterInstr instr;

	instr.BuildRol32Imm( 31, 1, 16 );

	EXPECT_EQ( InstrOp::ROL32_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 16, instr.args[2] );
}

TEST(InterInstr, Rol64Imm)
{
	InterInstr instr;

	instr.BuildRol64Imm( 31, 1, 16 );

	EXPECT_EQ( InstrOp::ROL64_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 16, instr.args[2] );
}

TEST(InterInstr, Sll32)
{
	InterInstr instr;

	instr.BuildSll32( 31, 1, 16 );

	EXPECT_EQ( InstrOp::SLL32, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 16, instr.args[2] );
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

TEST(InterInstr, Slr32Imm)
{
	InterInstr instr;

	instr.BuildSlr32Imm( 31, 1, 2 );

	EXPECT_EQ( InstrOp::SLR32_IMM, instr.op );
	EXPECT_EQ( 31, instr.args[0] );
	EXPECT_EQ( 1, instr.args[1] );
	EXPECT_EQ( 2, instr.args[2] );
}

TEST(InterInstr, Slr64Imm)
{
	InterInstr instr;

	instr.BuildSlr64Imm( 31, 1, 2 );

	EXPECT_EQ( InstrOp::SLR64_IMM, instr.op );
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

TEST(InterInstr, PpcStwcx)
{
	InterInstr instr;

	instr.BuildPpcStwcx( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_STWCX, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmpd)
{
	InterInstr instr;

	instr.BuildPpcCmpd( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPD, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmpdi)
{
	InterInstr instr;

	instr.BuildPpcCmpdi( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPDI, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmpw)
{
	InterInstr instr;

	instr.BuildPpcCmpw( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPW, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmpwi)
{
	InterInstr instr;

	instr.BuildPpcCmpwi( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPWI, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmpld)
{
	InterInstr instr;

	instr.BuildPpcCmpld( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPLD, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmpldi)
{
	InterInstr instr;

	instr.BuildPpcCmpldi( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPLDI, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmplw)
{
	InterInstr instr;

	instr.BuildPpcCmplw( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPLW, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcCmplwi)
{
	InterInstr instr;

	instr.BuildPpcCmplwi( 1, 2, 3 );

	EXPECT_EQ( InstrOp::PPC_CMPLWI, instr.op );
	EXPECT_EQ( 1, instr.args[0] );
	EXPECT_EQ( 2, instr.args[1] );
	EXPECT_EQ( 3, instr.args[2] );
}

TEST(InterInstr, PpcRfid)
{
	InterInstr instr;

	instr.BuildPpcRfid();

	EXPECT_EQ( InstrOp::PPC_RFID, instr.op );
}

