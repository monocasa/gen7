#include "jit/powerpc/PowerPCIntermediateBuilder.h"
#include "jit/InterInstr.h"

#include <gtest/gtest.h>

using namespace jit;

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

