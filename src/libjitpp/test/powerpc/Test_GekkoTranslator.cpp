#include "jitpp/powerpc/GekkoCpuContext.h"
#include "jitpp/powerpc/GekkoTranslator.h"

#include <gtest/gtest.h>

using jitpp::CommonOp;
using jitpp::GekkoCpuContext;
using jitpp::GekkoTranslator;

TEST(GekkoTranslator, Branch)
{
	GekkoTranslator translator;
	CommonOp ops[10];

	//standard branch
	ASSERT_EQ( 1, translator.BuildOps(ops, 0x48000100, 0x00001000) );

	EXPECT_EQ( CommonOp::BRANCH,    ops[0].type );
	EXPECT_EQ( CommonOp::Arg::ADDR, ops[0].args[0].type );
	EXPECT_EQ( 0x00001100,          ops[0].args[0].addr );

	//Branch and Link
	ASSERT_EQ( 2, translator.BuildOps(ops, 0x48000101, 0x00001000) );

	EXPECT_EQ( CommonOp::LOAD_REG,      ops[0].type );
	EXPECT_EQ( CommonOp::Arg::GPR_32,   ops[0].args[0].type );
	EXPECT_EQ( GekkoCpuContext::GPR_LR, ops[0].args[0].reg );
	EXPECT_EQ( CommonOp::Arg::IMM_U32,  ops[0].args[1].type );
	EXPECT_EQ( 0x00001004,              ops[0].args[1].u32 );

	EXPECT_EQ( CommonOp::BRANCH,    ops[1].type );
	EXPECT_EQ( CommonOp::Arg::ADDR, ops[1].args[0].type );
	EXPECT_EQ( 0x00001100,          ops[1].args[0].addr );
}

TEST(GekkoTranslator, Mflr)
{
	GekkoTranslator translator;
	CommonOp ops[10];

	// mflr     r31
	ASSERT_EQ( 1, translator.BuildOps(ops, 0x7FE802A6, 0x00000000) );

	EXPECT_EQ( CommonOp::LOAD_REG,      ops[0].type );
	EXPECT_EQ( CommonOp::Arg::GPR_32,   ops[0].args[0].type );
	EXPECT_EQ( 31,                      ops[0].args[0].reg );
	EXPECT_EQ( CommonOp::Arg::GPR_32,   ops[0].args[1].type );
	EXPECT_EQ( GekkoCpuContext::GPR_LR, ops[0].args[1].u32 );
}

TEST(GekkoTranslator, Oris)
{
	GekkoTranslator translator;
	CommonOp ops[10];

	// oris     r31, r32, 0x8000
	ASSERT_EQ( 1, translator.BuildOps(ops, 0x67ff8000, 0x00000000) );

	EXPECT_EQ( CommonOp::OR,            ops[0].type );
	EXPECT_EQ( CommonOp::Arg::GPR_32,   ops[0].args[0].type );
	EXPECT_EQ( 31,                      ops[0].args[0].reg );
	EXPECT_EQ( CommonOp::Arg::GPR_32,   ops[0].args[1].type );
	EXPECT_EQ( 31,                      ops[0].args[1].reg );
	EXPECT_EQ( CommonOp::Arg::IMM_U32,  ops[0].args[2].type );
	EXPECT_EQ( 0x80000000,              ops[0].args[2].u32 );
}

