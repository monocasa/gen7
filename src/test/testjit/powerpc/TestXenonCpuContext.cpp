#include "jit/powerpc/XenonCpuContext.h"

#include <gtest/gtest.h>

TEST(XenonCpuContext, ConditionRegister)
{
	jit::XenonCpuContext context( 0 );
	context.cr = 0;

	context.SetCrSo( 0 );
	context.SetCrEq( 1 );
	context.SetCrGt( 2 );
	context.SetCrLt( 3 );

	EXPECT_EQ( 0x12480000, context.cr );

	EXPECT_EQ( 1, context.ReadCr( 0 ) ); 
	EXPECT_EQ( 2, context.ReadCr( 1 ) );
	EXPECT_EQ( 4, context.ReadCr( 2 ) );
	EXPECT_EQ( 8, context.ReadCr( 3 ) );

	context.ClearCr( 1 );
	context.ClearCr( 2 );

	EXPECT_EQ( 0x10080000, context.cr );
}

TEST(XenonCpuContext, Msr)
{
	jit::XenonCpuContext context( 0 );

	context.msr = 0x0000000000000000UL;
	EXPECT_FALSE( context.Is64Bit() );

	context.msr = 0x8000000000000000UL;
	EXPECT_TRUE( context.Is64Bit() );
}

