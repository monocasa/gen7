#include "jitpp/CommonOp.h"

#include <gtest/gtest.h>

using jitpp::CommonOp;

TEST(CommonOp, Unknown)
{
	CommonOp op = CommonOp::BuildUnknown( 0xFFFFFFFF, 0xEEEEEEEE, 0x0123456789abcdefUL );
	EXPECT_EQ( CommonOp::UNKNOWN, op.type );
	EXPECT_EQ( nullptr, op.next );

	EXPECT_EQ( CommonOp::Arg::IMM_MISC, op.args[0].type );
	EXPECT_EQ( 0xFFFFFFFF,              op.args[0].misc );
	EXPECT_EQ( CommonOp::Arg::IMM_MISC, op.args[1].type );
	EXPECT_EQ( 0xEEEEEEEE,              op.args[1].misc );
	EXPECT_EQ( CommonOp::Arg::ADDR,     op.args[2].type );
	EXPECT_EQ( 0x0123456789abcdefUL,    op.args[2].addr );
}

TEST(CommonOp, Invalid)
{
	CommonOp op = CommonOp::BuildInvalid( 0x0123456789abcdefUL );
	EXPECT_EQ( CommonOp::INVALID, op.type );
	EXPECT_EQ( nullptr, op.next );

	EXPECT_EQ( CommonOp::Arg::ADDR,  op.args[0].type );
	EXPECT_EQ( 0x0123456789abcdefUL, op.args[0].addr );
}

//test nop

TEST(CommonOp, LoadRegGpr32ImmU32)
{
	CommonOp op = CommonOp::BuildLoadRegGpr32ImmU32( 4, 0xFEDCBA98 );
	EXPECT_EQ( CommonOp::LOAD_REG, op.type );
	EXPECT_EQ( nullptr, op.next );

	EXPECT_EQ( CommonOp::Arg::GPR_32,  op.args[0].type );
	EXPECT_EQ( 4,                      op.args[0].reg );
	EXPECT_EQ( CommonOp::Arg::IMM_U32, op.args[1].type );
	EXPECT_EQ( 0xFEDCBA98,             op.args[1].u32 );
}

TEST(CommonOp, Branch)
{
	CommonOp op = CommonOp::BuildBranch( 0x0123456789abcdefUL );
	EXPECT_EQ( CommonOp::BRANCH, op.type );
	EXPECT_EQ( nullptr, op.next );

	EXPECT_EQ( CommonOp::Arg::ADDR,  op.args[0].type );
	EXPECT_EQ( 0x0123456789abcdefUL, op.args[0].addr );
}

TEST(CommonOp, OrImm32)
{
	CommonOp op = CommonOp::BuildOrImm32( 1, 2, 0xFFAABBCC );
	EXPECT_EQ( CommonOp::OR, op.type );
	EXPECT_EQ( nullptr, op.next );

	EXPECT_EQ( CommonOp::Arg::GPR_32,  op.args[0].type );
	EXPECT_EQ( 1,                      op.args[0].reg );
	EXPECT_EQ( CommonOp::Arg::GPR_32,  op.args[1].type );
	EXPECT_EQ( 2,                      op.args[1].reg );
	EXPECT_EQ( CommonOp::Arg::IMM_U32, op.args[2].type );
	EXPECT_EQ( 0xFFAABBCC,             op.args[2].u32 );
}

