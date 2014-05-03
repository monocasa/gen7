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

