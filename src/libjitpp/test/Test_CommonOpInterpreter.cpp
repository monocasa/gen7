#include "jitpp/CommonOp.h"
#include "jitpp/CommonOpInterpreter.h"

#include "util/Compilers.h"

#include <gtest/gtest.h>

using jitpp::CommonOp;

class TestMemoryPolicy
{
};

class TestCommonOpInterpreter : public jitpp::CommonOpInterpreter<TestMemoryPolicy>
{
public:
	static const size_t GPR_SIZE = 64 * sizeof(uint64_t);

	std::vector<uint64_t> branches;

	uint8_t gprs[ GPR_SIZE ];

	uint32_t ReadGpr32( int reg ) {
		uint32_t *gpr32 = (uint32_t*)gprs;
		return gpr32[ reg ];
	}

	void SetPc( uint64_t newPc ) override final {
		branches.push_back( newPc );
	}

	void Reset() {
		branches.clear();
		memset( gprs, 0, 64 * 8 );
	}

	TestCommonOpInterpreter()
	  : CommonOpInterpreter( gprs )
	{
		Reset();
	}
};

TEST(CommonOpInterpreter, Uninitialized)
{
	TestCommonOpInterpreter interp;
	CommonOp op;

	EXPECT_FALSE( interp.ExecuteOp(op) );	
	EXPECT_STREQ( "Unknown CommonOp::Type:  0", interp.GetErrorString() );
}

TEST(CommonOpInterpreter, LoadReg)
{
	TestCommonOpInterpreter interp;
	CommonOp op = CommonOp::BuildLoadRegGpr32ImmU32( 4, 0xFEDCBA98 );

	EXPECT_TRUE( interp.ExecuteOp(op) );
	EXPECT_EQ( 0xFEDCBA98, interp.ReadGpr32(4) );
}

TEST(CommonOpInterpreter, Branch)
{
	TestCommonOpInterpreter interp;
	CommonOp op = CommonOp::BuildBranch( 0x1100 );

	EXPECT_TRUE( interp.ExecuteOp(op) );
	ASSERT_EQ( 1, interp.branches.size() );
	EXPECT_EQ( 0x1100, interp.branches[0] );
}

