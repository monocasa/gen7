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

TEST(PowerPCIntermediateBuilder, Addi)
{
	PowerPCIntermediateBuilder builder;
	InterInstr instr[10];

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x38600001, 0 ) );
	EXPECT_EQ( InstrOp::LOAD_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x00000001, instr[0].args[1] );

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3860FFFF, 0 ) );
	EXPECT_EQ( InstrOp::LOAD_IMM, instr[0].op );
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
	EXPECT_EQ( InstrOp::LOAD_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x00010000, instr[0].args[1] );

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3C60FFFF, 0 ) );
	EXPECT_EQ( InstrOp::LOAD_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0xFFFFFFFFFFFF0000UL, instr[0].args[1] );

	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x3c631000, 0 ) );
	EXPECT_EQ( InstrOp::ADD_IMM, instr[0].op );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 3 * sizeof(uint64_t), instr[0].args[1] );
	EXPECT_EQ( 0x10000000, instr[0].args[2] );
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
	EXPECT_EQ( InstrOp::LOAD_IMM, instr[0].op );
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
	EXPECT_EQ( InstrOp::LOAD_IMM, instr[0].op );
	EXPECT_EQ( 32 * sizeof(uint64_t), instr[0].args[0] );
	EXPECT_EQ( 0x00001004, instr[0].args[1] );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[1].op );
	EXPECT_EQ( 0x00000100, instr[1].args[0] );

	// 00002000 : 4BFFFFFC : b        loc_1ffc
	EXPECT_EQ( 1, builder.BuildIntermediate( instr, 0x4BFFFFFC, 0x00002000 ) );
	EXPECT_EQ( InstrOp::BRANCH_ALWAYS, instr[0].op );
	EXPECT_EQ( 0x00001FFC, instr[0].args[0] );
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

