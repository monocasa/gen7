#include "jit/powerpc/PowerPCIntermediateBuilder.h"
#include "jit/InterInstr.h"

#include <gtest/gtest.h>

using namespace jit;

TEST(PowerPCIntermediateBuilder, Unknown)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x03FFFFFF, 0x100 ) );
	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr[0].op );
	EXPECT_EQ( 0,          *instr[0].unknown.opcodeCookie );
	EXPECT_EQ( 0x03FFFFFF, *instr[0].unknown.instruction );
	EXPECT_EQ( 0x100,      *instr[0].unknown.pc );
}

TEST(PowerPCIntermediateBuilder, Add)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7ca53214 : add      r5, r5, r6
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7ca53214, 0 ) );
	EXPECT_EQ( InstrOp::ADD, instr[0].op );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 6 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[2] );

	// 00000000 : 7fc0f214 : add      r30, r0, r30
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7fc0f214, 0 ) );
	EXPECT_EQ( InstrOp::ADD, instr[0].op );
	EXPECT_EQ( 0  * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 30 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 30 * sizeof(uint64_t), instr[0].args[2] );
	

	// 00000000 : 7ca53215 : add.     r5, r5, r6
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7ca53215, 0 ) );
	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Addi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 38600001 : li       r3, 1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x38600001, 0x00000000 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM,   instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR64,        instr[0].imm64.dest.type );
	EXPECT_EQ( 0x00000001,           *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,          instr[0].imm64.source.type );

	// 00000000 : 3860FFFF : li       r3, -1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3860FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM,   instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR64,        instr[0].imm64.dest.type );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,          instr[0].imm64.source.type );

	// 00000000 : 38631000 : addi     r3, r3, 0x1000
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x38631000, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ADD_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x00001000, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Addis)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 3c600001 : lis      r3, 1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3c600001, 0x00000000 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM,   instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR64,        instr[0].imm64.dest.type );
	EXPECT_EQ( 0x00010000,           *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,          instr[0].imm64.source.type );

	// 00000000 : 3C60FFFF : lis      r3, 0xFFFF
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3C60FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM,   instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR64,        instr[0].imm64.dest.type );
	EXPECT_EQ( 0xFFFFFFFFFFFF0000UL, *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,          instr[0].imm64.source.type );

	// 00000000 : 3C631000 : addis    r3, r3, 0x1000
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3c631000, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ADD_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x10000000, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Andc)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c632078 : andc     r3, r3, r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c632078, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ANDC, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[2] );

	// 00000000 : 7c632079 : andc.    r3, r3, r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c632079, 0x00000000 ) );
	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Bcl)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00002000 : 429f0005 : bcl      loc_2004
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x429f0005, 0x00002000 ) );

	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[0].op );
	EXPECT_EQ( 0x2004, instr[0].args[0] );

	EXPECT_EQ( InstrOp::LD_64_IMM,    instr[1].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), *instr[1].imm64.dest );
	EXPECT_EQ( OpType::GPR64,         instr[1].imm64.dest.type );
	EXPECT_EQ( 0x0000000000002004UL,  *instr[1].imm64.source );
	EXPECT_EQ( OpType::IMM,           instr[1].imm64.source.type );
}

TEST(PowerPCIntermediateBuilder, Bctr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00002000 : 4e800421 : bctrl
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x4e800421, 0x00002000 ) );

	EXPECT_EQ( InstrOp::BRANCH_GPR64, instr[0].op );
	EXPECT_EQ( 33 * sizeof(uint64_t), instr[0].args[0] );

	EXPECT_EQ( InstrOp::LD_64_IMM,    instr[1].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), *instr[1].imm64.dest );
	EXPECT_EQ( OpType::GPR64,         instr[1].imm64.dest.type );
	EXPECT_EQ( 0x0000000000002004UL,  *instr[1].imm64.source );
	EXPECT_EQ( OpType::IMM,           instr[1].imm64.source.type );
}

TEST(PowerPCIntermediateBuilder, Bclr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 4d9e0020 : beqlr    cr7
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x4d9e0020, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 38 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 2, instr[0].args[2] );
	EXPECT_EQ( InstrOp::BRANCH_GPR64_NOT_ZERO_GPR64, instr[1].op );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 32 * sizeof(uint64_t), instr[1].args[1] );
}

TEST(PowerPCIntermediateBuilder, Bdnz)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00002000 : 4320fffc : bdnz+    loc_1ffc
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x4320fffc, 0x00002000 ) );
	EXPECT_EQ( InstrOp::SUBU_IMM, instr[0].op );
	EXPECT_EQ( 33 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 33 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 1, instr[0].args[2] );
	EXPECT_EQ( InstrOp::BRANCH_GPR64_NOT_ZERO, instr[1].op );
	EXPECT_EQ( 33 * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 0x1ffc, instr[1].args[1] );
}

TEST(PowerPCIntermediateBuilder, Beq)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00002dd8 : 4182000c : beq      loc_2de4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4182000c, 0x00002dd8 ) );
	EXPECT_EQ( InstrOp::BRANCH_GPR32_MASK_NOT_ZERO, instr[0].op );
	EXPECT_EQ( 38 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x20000000, instr[0].args[1] );
	EXPECT_EQ( 0x2de4, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Bne)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00002000 : 40c2fffc : bne-     loc_1ffc
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x40c2fffc, 0x00002000 ) );
	EXPECT_EQ( InstrOp::BRANCH_GPR32_MASK_ZERO, instr[0].op );
	EXPECT_EQ( 38 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x20000000, instr[0].args[1] );
	EXPECT_EQ( 0x1ffc, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Blr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 4e800020 : blr
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4e800020, 0x00000000 ) );
	EXPECT_EQ( InstrOp::BRANCH_GPR64, instr[0].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), instr[0].args[0] );
}

TEST(PowerPCIntermediateBuilder, Branch)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00001000 : 48000100 : b        loc_1100
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x48000100, 0x00001000 ) );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[0].op );
	EXPECT_EQ( 0x00001100, instr[0].args[0] );

	// 00001000 : 48000101 : bl       loc_1100
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x48000101, 0x00001000 ) );

	EXPECT_EQ( InstrOp::LD_64_IMM,    instr[0].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR64,         instr[0].imm64.dest.type );
	EXPECT_EQ( 0x0000000000001004UL,  *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,           instr[0].imm64.source.type );

	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[1].op );
	EXPECT_EQ( 0x00001100, instr[1].args[0] );

	// 00001000 : 48000102 : ba       loc_100
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x48000102, 0x00001000 ) );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[0].op );
	EXPECT_EQ( 0x00000100, instr[0].args[0] );

	// 00001000 : 48000103 : bla      loc_100
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x48000103, 0x00001000 ) );

	EXPECT_EQ( InstrOp::LD_64_IMM,    instr[0].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR64,         instr[0].imm64.dest.type );
	EXPECT_EQ( 0x0000000000001004UL,  *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,           instr[0].imm64.source.type );

	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[1].op );
	EXPECT_EQ( 0x00000100, instr[1].args[0] );

	// 00002000 : 4BFFFFFC : b        loc_1ffc
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4BFFFFFC, 0x00002000 ) );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[0].op );
	EXPECT_EQ( 0x00001FFC, instr[0].args[0] );
}

TEST(PowerPCIntermediateBuilder, Clrrwi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 541f0038 : clrrwi   r31, r0, 3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x541f0038, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 0  * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 31 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x00000000FFFFFFF8, instr[0].args[2] );

	// 00000000 : 5529003a : clrrwi   r9, r9, 2
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x5529003a, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 9 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x00000000FFFFFFFC, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmpd)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7FBB4800 : cmpd     cr7, r27, r9
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7FBB4800, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPD, instr[0].op );
	EXPECT_EQ( 27 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9  * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 7, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmpdi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 2C23FFFF : cmpdi    r3, -1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x2C23FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPDI, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0, instr[0].args[1] );
	EXPECT_EQ( -1, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmpw)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7f9b4800 : cmpw     cr7, r27, r9
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7f9b4800, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPW, instr[0].op );
	EXPECT_EQ( 27 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9  * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 7, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmpwi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 2F800000 : cmpwi    cr7, r0, 0
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x2F800000, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPWI, instr[0].op );
	EXPECT_EQ( 0 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 7, instr[0].args[1] );
	EXPECT_EQ( 0, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmpld)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7fa4f840 : cmpld    cr7, r4, r31
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7fa4f840, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPLD, instr[0].op );
	EXPECT_EQ( 4  * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 31 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 7, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmpldi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 2823FFFF : cmpldi   r3, 0xFFFF
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x2823FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPLDI, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0, instr[0].args[1] );
	EXPECT_EQ( 0xFFFF, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmplw)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7f84f840 : cmplw    cr7, r4, r31
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7f84f840, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPLW, instr[0].op );
	EXPECT_EQ( 4  * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 31 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 7, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmplwi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 2803FFFF : cmplwi   r3, 0xFFF
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x2803FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPLWI, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0, instr[0].args[1] );
	EXPECT_EQ( 0xFFFF, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Crxor)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 4cc63182 : crclr    4*cr1+eq
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4cc63182, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_32, instr[0].op );
	EXPECT_EQ( 38 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 38 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0xFDFFFFFF, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Eieio)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c0006ac : eieio
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c0006ac, 0x00000000 ) );
	EXPECT_EQ( InstrOp::NOP, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Isync)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 4c00012c : isync
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4c00012c, 0x00000000 ) );
	EXPECT_EQ( InstrOp::NOP, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Ld)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : e8bf0028 : ld       r5, 0x28(r31)
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0xe8bf0028, 0x00000000 ) );
	EXPECT_EQ( InstrOp::LD_64_REG_OFF, instr[0].op );
	EXPECT_EQ( 5 * sizeof(uint64_t),   *instr[0].ldStIdx.valueReg );
	EXPECT_EQ( OpType::GPR64,          instr[0].ldStIdx.valueReg.type );
	EXPECT_EQ( 31 * sizeof(uint64_t),  *instr[0].ldStIdx.addrReg );
	EXPECT_EQ( OpType::GPR64,          instr[0].ldStIdx.addrReg.type );
	EXPECT_EQ( 0x28,                   *instr[0].ldStIdx.offset );
	EXPECT_EQ( OpType::IMM,            instr[0].ldStIdx.offset.type );
}

TEST(PowerPCIntermediateBuilder, Lwarx)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7cc01828 : lwarx    r6, 0, r3
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x7cc01828, 0x00000000 ) );

	EXPECT_EQ( InstrOp::LD_32_L,     instr[0].op );
	EXPECT_EQ( 6 * sizeof(uint64_t), *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR32,        instr[0].twoReg.dest.type );
	EXPECT_EQ( 3 * sizeof(uint64_t), *instr[0].twoReg.source );
	EXPECT_EQ( OpType::GPR64,        instr[0].twoReg.source.type );

	EXPECT_EQ( InstrOp::LD_32_IMM,                        instr[1].op );
	EXPECT_EQ( (6 * sizeof(uint64_t)) + sizeof(uint32_t), *instr[1].imm32.dest );
	EXPECT_EQ( OpType::GPR32,                             instr[1].imm32.dest.type );
	EXPECT_EQ( 0,                                         *instr[1].imm32.source );
	EXPECT_EQ( OpType::IMM,                               instr[1].imm32.source.type );

	// 00000000 : 7cc51828 : lwarx    r6, r5, r3
	EXPECT_EQ( 3, builder.BuildIntermediate( instr, 0x7cc51828, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ADD,          instr[0].op );
	EXPECT_EQ( 5  * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3  * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[0].args[2] );

	EXPECT_EQ( InstrOp::LD_32_L,      instr[1].op );
	EXPECT_EQ( 6  * sizeof(uint64_t), *instr[1].twoReg.dest );
	EXPECT_EQ( OpType::GPR32,         instr[1].twoReg.dest.type );
	EXPECT_EQ( 40 * sizeof(uint64_t), *instr[1].twoReg.source );
	EXPECT_EQ( OpType::GPR64,         instr[1].twoReg.source.type );

	EXPECT_EQ( InstrOp::LD_32_IMM,                        instr[2].op );
	EXPECT_EQ( (6 * sizeof(uint64_t)) + sizeof(uint32_t), *instr[2].imm32.dest );
	EXPECT_EQ( OpType::GPR32,                             instr[2].imm32.dest.type );
	EXPECT_EQ( 0,                                         *instr[2].imm32.source );
	EXPECT_EQ( OpType::IMM,                               instr[2].imm32.source.type );
}

TEST(PowerPCIntermedaiteBuilder, Lwbrx)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c601c2c : lwbrx    r3, r0, r3
	EXPECT_EQ( 3, builder.BuildIntermediate( instr, 0x7c601c2c, 0x00000000 ) );

	EXPECT_EQ( InstrOp::LD_32_REG_OFF, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t),   *instr[0].ldStIdx.valueReg );
	EXPECT_EQ( OpType::GPR32,          instr[0].ldStIdx.valueReg.type );
	EXPECT_EQ( 3 * sizeof(uint64_t),   *instr[0].ldStIdx.addrReg );
	EXPECT_EQ( OpType::GPR64,          instr[0].ldStIdx.addrReg.type );
	EXPECT_EQ( 0,                      *instr[0].ldStIdx.offset );
	EXPECT_EQ( OpType::IMM,            instr[0].ldStIdx.offset.type );

	EXPECT_EQ( InstrOp::BYTE_SWAP_32, instr[1].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[1].args[1] );

	EXPECT_EQ( InstrOp::LD_32_IMM,                        instr[2].op );
	EXPECT_EQ( (3 * sizeof(uint64_t)) + sizeof(uint32_t), *instr[2].imm32.dest );
	EXPECT_EQ( OpType::GPR32,                             instr[2].imm32.dest.type );
	EXPECT_EQ( 0,                                         *instr[2].imm32.source );
	EXPECT_EQ( OpType::IMM,                               instr[2].imm32.source.type );

}

TEST(PowerPCIntermediateBuilder, Lwz)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 80841e1c : lwz      r4, 0x1e1c(r4)
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x80841e1c, 0x00000000 ) );

	EXPECT_EQ( InstrOp::LD_32_REG_OFF, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t),   *instr[0].ldStIdx.valueReg );
	EXPECT_EQ( OpType::GPR32,          instr[0].ldStIdx.valueReg.type );
	EXPECT_EQ( 4 * sizeof(uint64_t),   *instr[0].ldStIdx.addrReg );
	EXPECT_EQ( OpType::GPR64,          instr[0].ldStIdx.addrReg.type );
	EXPECT_EQ( 0x1e1c,                 *instr[0].ldStIdx.offset );
	EXPECT_EQ( OpType::IMM,            instr[0].ldStIdx.offset.type );

	EXPECT_EQ( InstrOp::LD_32_IMM,                        instr[1].op );
	EXPECT_EQ( (4 * sizeof(uint64_t)) + sizeof(uint32_t), *instr[1].imm32.dest );
	EXPECT_EQ( OpType::GPR32,                             instr[1].imm32.dest.type );
	EXPECT_EQ( 0,                                         *instr[1].imm32.source );
	EXPECT_EQ( OpType::IMM,                               instr[1].imm32.source.type );
}

TEST(PowerPCIntermediateBuilder, Mtspr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7C79FBA6 : mtspr    hid6, r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C79FBA6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG,    instr[0].op );
	EXPECT_EQ( 1017,                 *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::SYS64,        instr[0].twoReg.dest.type );
	EXPECT_EQ( 3 * sizeof(uint64_t), *instr[0].twoReg.source );
	EXPECT_EQ( OpType::GPR64,        instr[0].twoReg.source.type );

	// 00000000 : 7C6803A6 : mtlr     r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C6803A6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG,     instr[0].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR64,         instr[0].twoReg.dest.type );
	EXPECT_EQ( 3  * sizeof(uint64_t), *instr[0].twoReg.source );
	EXPECT_EQ( OpType::GPR64,         instr[0].twoReg.source.type );

	// 00000000 : 7C8903A6 : mtctr    r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C8903A6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG,     instr[0].op );
	EXPECT_EQ( 33 * sizeof(uint64_t), *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR64,         instr[0].twoReg.dest.type );
	EXPECT_EQ( 4  * sizeof(uint64_t), *instr[0].twoReg.source );
	EXPECT_EQ( OpType::GPR64,         instr[0].twoReg.source.type );

	// 00000000 : 7C7143A6 : mtsprg1   r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C7143A6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG,     instr[0].op );
	EXPECT_EQ( 35 * sizeof(uint64_t), *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR64,         instr[0].twoReg.dest.type );
	EXPECT_EQ( 3  * sizeof(uint64_t), *instr[0].twoReg.source );
	EXPECT_EQ( OpType::GPR64,         instr[0].twoReg.source.type );
}

TEST(PowerPCIntermediateBuilder, Mfcr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c800026 : mfcr     r4
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x7c800026, 0x00000000 ) );

	EXPECT_EQ( InstrOp::MOVE_REG,     instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t),  *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR32,         instr[0].twoReg.dest.type );
	EXPECT_EQ( 38 * sizeof(uint64_t), *instr[0].twoReg.source );
	EXPECT_EQ( OpType::GPR32,         instr[0].twoReg.source.type );

	EXPECT_EQ( InstrOp::LD_32_IMM,                        instr[1].op );
	EXPECT_EQ( (4 * sizeof(uint64_t)) + sizeof(uint32_t), *instr[1].imm32.dest );
	EXPECT_EQ( OpType::GPR32,                             instr[1].imm32.dest.type );
	EXPECT_EQ( 0,                                         *instr[1].imm32.source );
	EXPECT_EQ( OpType::IMM,                               instr[1].imm32.source.type );

}

TEST(PowerPCIntermediateBuilder, Mfmsr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7ca000a6 : mfmsr    r5
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7ca000a6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG,    instr[0].op );
	EXPECT_EQ( 5 * sizeof(uint64_t), *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR64,        instr[0].twoReg.dest.type );
	EXPECT_EQ( 1024,                 *instr[0].twoReg.source );
	EXPECT_EQ( OpType::SYS64,        instr[0].twoReg.source.type );
}

TEST(PowerPCIntermediateBuilder, Mfspr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7C79FAA6 : mfspr    r3, hid6
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C79FAA6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG,    instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR64,        instr[0].twoReg.dest.type );
	EXPECT_EQ( 1017,                 *instr[0].twoReg.source );
	EXPECT_EQ( OpType::SYS64,        instr[0].twoReg.source.type );
}

TEST(PowerPCIntermediateBuilder, Mtcr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c8ff120 : mtcr     r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c8ff120, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG,     instr[0].op );
	EXPECT_EQ( 38 * sizeof(uint64_t), *instr[0].twoReg.dest );
	EXPECT_EQ( OpType::GPR32,         instr[0].twoReg.dest.type );
	EXPECT_EQ( 4 * sizeof(uint64_t),  *instr[0].twoReg.source );
	EXPECT_EQ( OpType::GPR32,         instr[0].twoReg.source.type );
}

TEST(PowerPCIntermediateBuilder, Rlwinm)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 5749077e : clrlwi   r9, r26, 29
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x5749077e, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 26 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9  * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x0000000000000007UL, instr[0].args[2] );

	// 00000000 : 5718053e : clrlwi   r24, r24, 20
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x5718053e, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 24 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 24 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x0000000000000FFFUL, instr[0].args[2] );

	// 00000000 : 57180026 : clrrwi   r24, r24, 12
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x57180026, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 24 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 24 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x00000000FFFFF000UL, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Rotate)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 78841764 : sldi     r4, r4, 2
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x78841764, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL64_IMM, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 2, instr[0].args[2] );

	// 00000000 : 786307C6 : sldi     r3, r3, 32
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x786307C6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 32, instr[0].args[2] );

	// 00000000 : 788464C6 : sldi     r4, r4, 44
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x788464C6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL64_IMM, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 44, instr[0].args[2] );

	// 00000000 : 7863F806 : sldi     r3, r3, 63
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7863F806, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 63, instr[0].args[2] );

	// 00000000 : 78A50040 : clrldi   r5, r5, 1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x78A50040, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x7FFFFFFFFFFFFFFFUL, instr[0].args[2] );

	// 00000000 : 78630120 : clrldi   r3, r3, 36
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x78630120, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x000000000FFFFFFFUL, instr[0].args[2] );

	// 00000000 : 788604a0 : clrldi   r6, r4, 50
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x788604a0, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 6 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x0000000000003FFFUL, instr[0].args[2] );

	// 00000000 : 786203e4 : clrrdi   r2, r3, 16
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x786203e4, 0x00000000 ) );
	EXPECT_EQ( InstrOp::AND_IMM_64, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 2 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0xFFFFFFFFFFFF0000UL, instr[0].args[2] );

	// 00000000 : 78855224 : extldi   r5, r4, 41, 10
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x78855224, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ROL64_IMM, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 10, instr[0].args[2] );

	EXPECT_EQ( InstrOp::AND_IMM_64, instr[1].op );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[1].args[1] );
	EXPECT_EQ( 0xFFFFFFFFFF800000UL, instr[1].args[2] );

	// 00000000 : 78642722 : extrdi   r4, r3, 4, 32
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x78642722, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ROL64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 36, instr[0].args[2] );

	EXPECT_EQ( InstrOp::AND_IMM_64, instr[1].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[1].args[1] );
	EXPECT_EQ( 0xF, instr[1].args[2] );

	// 00000000 : 7862a4e2 : extrdi   r2, r3, 13, 39
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x7862a4e2, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ROL64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 2 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 52, instr[0].args[2] );

	EXPECT_EQ( InstrOp::AND_IMM_64, instr[1].op );
	EXPECT_EQ( 2 * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 2 * sizeof(uint64_t), instr[1].args[1] );
	EXPECT_EQ( 0x01FFF, instr[1].args[2] );

	//// 00000000 : 78442c0e : insrdi   r4, r2, 11, 16
	EXPECT_EQ( 4, builder.BuildIntermediate( instr, 0x78442c0e, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ANDC_IMM, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x0000FFE000000000UL, instr[0].args[2] );

	EXPECT_EQ( InstrOp::ROL64_IMM, instr[1].op );
	EXPECT_EQ( 2 * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[1].args[1] );
	EXPECT_EQ( 37, instr[1].args[2] );

	EXPECT_EQ( InstrOp::AND_IMM_64, instr[2].op );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[2].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[2].args[1] );
	EXPECT_EQ( 0x0000FFE000000000UL, instr[2].args[2] );

	EXPECT_EQ( InstrOp::OR, instr[3].op );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[3].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[3].args[1] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[3].args[2] );
}

TEST(PowerPCIntermediateBuilder, Or)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c632378 : or       r3, r3, r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c632378, 0x00000000 ) );
	EXPECT_EQ( InstrOp::OR, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[2] );

	// 00000000 : 7c632379 : or.      r3, r3, r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c632379, 0x00000000 ) );
	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Ori)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 6064FFFF : ori      r4, r3, 0xFFFF
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x6064FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::OR_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x0000FFFF, instr[0].args[2] );

	// 00000000 : 60000000 : nop
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x60000000, 0x00000000 ) );
	EXPECT_EQ( InstrOp::NOP, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Oris)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 6464FFFF : oris     r4, r3, 0xFFFF
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x6464FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::OR_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0xFFFF0000, instr[0].args[2] );

	// 00000000 : 64000000 : nop
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x64000000, 0x00000000 ) );
	EXPECT_EQ( InstrOp::NOP, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Rfid)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 4c000024 : rfid
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4c000024, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_RFID, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Slbia)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7C0003e4 : slbia
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C0003e4, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_SLBIA, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Slbmte)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c802B24 : slbmte   r4, r5
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c802B24, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_SLBMTE, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[1] );
}

TEST(PowerPCIntermediateBuilder, Slw)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7ca51830 : slw      r5, r5, r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7ca51830, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL32, instr[0].op );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[2] );

	// 00000000 : 7ca51830 : slw      r5, r5, r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7ca51831, 0x00000000 ) );
	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Slwi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 5463482c : slwi     r3, r3, 9
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x5463482c, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL32_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 9, instr[0].args[2] );

	// 00000000 : 5463901a : slwi     r3, r3, 18
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x5463901a, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL32_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 18, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Srawi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7cc91670 : srawi    r9, r6, 2
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7cc91670, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_SRAWI, instr[0].op );
	EXPECT_EQ( 6 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 2, instr[0].args[2] );
	EXPECT_FALSE( instr[0].args[3] );

	// 00000000 : 7cc91671 : srawi.   r9, r6, 2
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7cc91671, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_SRAWI, instr[0].op );
	EXPECT_EQ( 6 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 2, instr[0].args[2] );
	EXPECT_TRUE( instr[0].args[3] );
}

TEST(PowerPCIntermediateBuilder, Srwi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 57e6e8fe : srwi     r6, r31, 3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x57e6e8fe, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLR32_IMM, instr[0].op );
	EXPECT_EQ( 31 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 6  * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 3, instr[0].args[2] );

	// 00000000 : 5463843e : srwi     r3, r3, 16
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x5463843e, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLR32_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 16, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Std)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : f83f0008 : std      r1, 8(r31)
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0xf83f0008, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ST_REG_OFF,   instr[0].op );
	EXPECT_EQ( 1 * sizeof(uint64_t),  *instr[0].ldStIdx.valueReg );
	EXPECT_EQ( OpType::GPR64,         instr[0].ldStIdx.valueReg.type );
	EXPECT_EQ( 31 * sizeof(uint64_t), *instr[0].ldStIdx.addrReg );
	EXPECT_EQ( OpType::GPR64,         instr[0].ldStIdx.addrReg.type );
	EXPECT_EQ( 8,                     *instr[0].ldStIdx.offset );
	EXPECT_EQ( OpType::IMM,           instr[0].ldStIdx.offset.type );

	// 00000000 : f8207ffc : std      r1, 0x7ffc
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0xf8207ffc, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ST_ABS,      instr[0].op );
	EXPECT_EQ( 1 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR64,        instr[0].imm64.dest.type );
	EXPECT_EQ( 0x7ffc,               *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,          instr[0].imm64.source.type );

	// 00000000 : f83ffffc : std      r1, -4(r31)
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0xf83ffffc, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ST_REG_OFF,   instr[0].op );
	EXPECT_EQ( 1 * sizeof(uint64_t),  *instr[0].ldStIdx.valueReg );
	EXPECT_EQ( OpType::GPR64,         instr[0].ldStIdx.valueReg.type );
	EXPECT_EQ( 31 * sizeof(uint64_t), *instr[0].ldStIdx.addrReg );
	EXPECT_EQ( OpType::GPR64,         instr[0].ldStIdx.addrReg.type );
	EXPECT_EQ( -4,                    *instr[0].ldStIdx.offset );
	EXPECT_EQ( OpType::IMM,           instr[0].ldStIdx.offset.type );
}

TEST(PowerPCIntermediateBuilder, Stb)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 98010008 : stb      r0, 8(r1)
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x98010008, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ADD_IMM, instr[0].op );
	EXPECT_EQ( 1  * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x0000000000000008UL, instr[0].args[2] );

	EXPECT_EQ( InstrOp::ST_8_REG, instr[1].op );
	EXPECT_EQ( 0  * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[1].args[1] );

	// 00000000 : 9801FFFF : stb      r0, -1(r1)
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x9801FFFF, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ADD_IMM, instr[0].op );
	EXPECT_EQ( 1  * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, instr[0].args[2] );

	EXPECT_EQ( InstrOp::ST_8_REG, instr[1].op );
	EXPECT_EQ( 0  * sizeof(uint64_t), instr[1].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[1].args[1] );

	// 00000000 : 98010000 : stb      r0, 0(r1)
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x98010000, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ST_8_REG, instr[0].op );
	EXPECT_EQ( 0 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 1 * sizeof(uint64_t), instr[0].args[1] );
}

TEST(PowerPCIntermediateBuilder, Stw)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 90200014 : stw      r1,0x14(0)
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x90200014, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ST_ABS,      instr[0].op );
	EXPECT_EQ( 1 * sizeof(uint64_t), *instr[0].imm64.dest );
	EXPECT_EQ( OpType::GPR32,        instr[0].imm64.dest.type );
	EXPECT_EQ( 0x14,                 *instr[0].imm64.source );
	EXPECT_EQ( OpType::IMM,          instr[0].imm64.source.type );

	// 00000000 : 90010014 : stw      r0,0x14(r1)
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x90010014, 0x00000000 ) );
	EXPECT_EQ( InstrOp::ST_REG_OFF,  instr[0].op );
	EXPECT_EQ( 0 * sizeof(uint64_t), *instr[0].ldStIdx.valueReg );
	EXPECT_EQ( OpType::GPR32,        instr[0].ldStIdx.valueReg.type );
	EXPECT_EQ( 1 * sizeof(uint64_t), *instr[0].ldStIdx.addrReg );
	EXPECT_EQ( OpType::GPR64,        instr[0].ldStIdx.addrReg.type );
	EXPECT_EQ( 0x14,                 *instr[0].ldStIdx.offset );
	EXPECT_EQ( OpType::IMM,          instr[0].ldStIdx.offset.type );
}

TEST(PowerPCIntermediateBuilder, Stwcx)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7cc0192d : stwcx.   r6, r0, r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7cc0192d, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_STWCX, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 6 * sizeof(uint64_t), instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Stwu)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// FFFFFFFF : 94000000 : stwu     r0,0(0) -- ra is not allowed to be 0
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x94000000, 0x00000000FFFFFFFFUL ) );
	EXPECT_EQ( InstrOp::INVALID_OPCODE, instr[0].op );
	EXPECT_EQ( 37,         *instr[0].unknown.opcodeCookie );
	EXPECT_EQ( 0x94000000, *instr[0].unknown.instruction );
	EXPECT_EQ( 0xFFFFFFFF, *instr[0].unknown.pc );

	// 00000000 : 943ffff0 : stwu     r1,-0x10(r31)
	EXPECT_EQ( 3, builder.BuildIntermediate( instr, 0x943ffff0, 0x00000000 ) );

	EXPECT_EQ( InstrOp::ADD_IMM, instr[0].op );
	EXPECT_EQ( 31 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 40 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( -0x10, instr[0].args[2] );

	EXPECT_EQ( InstrOp::ST_REG_OFF,   instr[1].op );
	EXPECT_EQ( 1  * sizeof(uint64_t), *instr[1].ldStIdx.valueReg );
	EXPECT_EQ( OpType::GPR32,         instr[1].ldStIdx.valueReg.type );
	EXPECT_EQ( 40 * sizeof(uint64_t), *instr[1].ldStIdx.addrReg );
	EXPECT_EQ( OpType::GPR64,         instr[1].ldStIdx.addrReg.type );
	EXPECT_EQ( 0,                     *instr[1].ldStIdx.offset );
	EXPECT_EQ( OpType::IMM,           instr[1].ldStIdx.offset.type );

	EXPECT_EQ( InstrOp::ADD_IMM, instr[2].op );
	EXPECT_EQ( 31 * sizeof(uint64_t), instr[2].args[0] );
	EXPECT_EQ( 31 * sizeof(uint64_t), instr[2].args[1] );
	EXPECT_EQ( -0x10, instr[2].args[2] );
}

TEST(PowerPCIntermediateBuilder, Subf)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c632050 : subf     r3, r3, r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c632050, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SUB, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[2] );

	// 00000000 : 7c230850 : subf     r1, r3, r1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c230850, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SUB, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 1 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 1 * sizeof(uint64_t), instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Subfic)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 21690008 : subfic   r11, r9, 8
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x21690008, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_SUBFIC, instr[0].op );
	EXPECT_EQ( 11 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9  * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x0000000000000008, instr[0].args[2] );

	// 00000000 : 2009ffff : subfic   r0, r9, -1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x2009ffff, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_SUBFIC, instr[0].op );
	EXPECT_EQ( 0 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 9 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( -1, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Sync)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7C0004AC : sync     0
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C0004AC, 0x00000000 ) );
	EXPECT_EQ( InstrOp::NOP, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Tlbiel)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7c001a24 : tlbiel   r3, 0
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c001a24, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_TLBIEL, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0, instr[0].args[1] );

	// 00000000 : 7c201a24 : tlbiel   r3, 1
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c201a24, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_TLBIEL, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 1, instr[0].args[1] );
}

TEST(PowerPCIntermediateBuilder, Xoris)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 6d605f61 : xoris    r0, r11, 0x5F61
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x6d605f61, 0x00000000 ) );
	EXPECT_EQ( InstrOp::XOR_IMM, instr[0].op );
	EXPECT_EQ( 11 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0  * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x000000005f610000UL, instr[0].args[2] );
}

