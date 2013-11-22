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
	EXPECT_EQ( 0,          instr[0].args[0] );
	EXPECT_EQ( 0x03FFFFFF, instr[0].args[1] );
	EXPECT_EQ( 0x100,      instr[0].args[2] );
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

	// 00000000 : 7ca53215 : add.     r5, r5, r6
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7ca53215, 0 ) );
	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr[0].op );
}

TEST(PowerPCIntermediateBuilder, Addi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x38600001, 0 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x00000001, instr[0].args[1] );

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3860FFFF, 0 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0xFFFFFFFFFFFFFFFFUL, instr[0].args[1] );

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x38631000, 0 ) );
	EXPECT_EQ( InstrOp::ADD_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x00001000, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Addis)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3c600001, 0 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x00010000, instr[0].args[1] );

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3C60FFFF, 0 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0xFFFFFFFFFFFF0000UL, instr[0].args[1] );

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3c631000, 0 ) );
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
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c632078, 0 ) );
	EXPECT_EQ( InstrOp::ANDC, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[2] );

	// 00000000 : 7c632079 : andc.    r3, r3, r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7c632079, 0 ) );
	EXPECT_EQ( InstrOp::UNKNOWN_OPCODE, instr[0].op );
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

TEST(PowerPcIntermediateBuilder, Bne)
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
	EXPECT_EQ( InstrOp::LD_64_IMM, instr[0].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x00001004, instr[0].args[1] );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[1].op );
	EXPECT_EQ( 0x00001100, instr[1].args[0] );

	// 00001000 : 48000102 : ba       loc_100
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x48000102, 0x00001000 ) );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[0].op );
	EXPECT_EQ( 0x00000100, instr[0].args[0] );

	// 00001000 : 48000103 : bla      loc_100
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x48000103, 0x00001000 ) );
	EXPECT_EQ( InstrOp::LD_64_IMM, instr[0].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x00001004, instr[0].args[1] );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[1].op );
	EXPECT_EQ( 0x00000100, instr[1].args[0] );

	// 00002000 : 4BFFFFFC : b        loc_1ffc
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4BFFFFFC, 0x00002000 ) );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[0].op );
	EXPECT_EQ( 0x00001FFC, instr[0].args[0] );
}

TEST(PowerPCIntermediateBuilder, Cmpldi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 2823FFFF : cmpldi   r3, 0
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x2823FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPLDI, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0, instr[0].args[1] );
	EXPECT_EQ( 0xFFFF, instr[0].args[2] );
}

TEST(PowerPCIntermediateBuilder, Cmplwi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 2803FFFF : cmplwi   r3, 0
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x2803FFFF, 0x00000000 ) );
	EXPECT_EQ( InstrOp::PPC_CMPLWI, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0, instr[0].args[1] );
	EXPECT_EQ( 0xFFFF, instr[0].args[2] );
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

TEST(PowerPCIntermediateBuilder, Lwarx)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7cc01828 : lwarx    r6, r0, r3
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x7cc01828, 0x00000000 ) );
	EXPECT_EQ( InstrOp::LD_32_L, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 6 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( InstrOp::LD_32_IMM, instr[1].op );
	EXPECT_EQ( (6 * sizeof(uint64_t)) + sizeof(uint32_t), instr[1].args[0] );
	EXPECT_EQ( 0, instr[1].args[1] );

	// 00000000 : 7cc51828 : lwarx    r6, r5, r3
	EXPECT_EQ( 2, builder.BuildIntermediate( instr, 0x7cc51828, 0x00000000 ) );
	EXPECT_EQ( InstrOp::LD_32_IDX_L, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 6 * sizeof(uint64_t), instr[0].args[2] );
	EXPECT_EQ( InstrOp::LD_32_IMM, instr[1].op );
	EXPECT_EQ( (6 * sizeof(uint64_t)) + sizeof(uint32_t), instr[1].args[0] );
	EXPECT_EQ( 0, instr[1].args[1] );
}

TEST(PowerPCIntermediateBuilder, Mtspr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7C79FBA6 : mtspr    hid6, r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C79FBA6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SET_SYS_REG, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 1017, instr[0].args[1] );

	// 00000000 : 7C6803A6 : mtlr     r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C6803A6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 32 * sizeof(uint64_t), instr[0].args[1] );

	// 00000000 : 7C8903A6 : mtctr    r4
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C8903A6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG, instr[0].op );
	EXPECT_EQ( 4 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 33 * sizeof(uint64_t), instr[0].args[1] );

	// 00000000 : 7C7143A6 : mtsprg1   r3
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C7143A6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::MOVE_REG, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 35 * sizeof(uint64_t), instr[0].args[1] );
}

TEST(PowerPCIntermediateBuilder, Mfmsr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7ca000a6 : mfmsr    r5
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7ca000a6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::READ_SYS, instr[0].op );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 1024, instr[0].args[1] );
}

TEST(PowerPCIntermediateBuilder, Mfspr)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	// 00000000 : 7C79FAA6 : mfspr    r3, hid6
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x7C79FAA6, 0x00000000 ) );
	EXPECT_EQ( InstrOp::READ_SYS, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 1017, instr[0].args[1] );
}

TEST(PowerPCIntermediateBuilder, Rotate)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

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
	EXPECT_EQ( InstrOp::AND_IMM, instr[0].op );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 5 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x7FFFFFFFFFFFFFFFUL, instr[0].args[2] );
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

	// 00000000 : 5463901a : slwi     r3, r3, 18
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x5463901a, 0x00000000 ) );
	EXPECT_EQ( InstrOp::SLL32_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 18, instr[0].args[2] );
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

