#include "jit/powerpc/XenonCpuContext.h"

#include <gtest/gtest.h>

TEST(XenonCpuContext, ConditionRegister)
{
	jit::ConditionRegister cr;

	cr.nybble = 0;
	cr.so = 1;
	EXPECT_EQ( 1, cr.nybble );

	cr.nybble = 0;
	cr.eq = 1;
	EXPECT_EQ( 2, cr.nybble );

	cr.nybble = 0;
	cr.gt = 1;
	EXPECT_EQ( 4, cr.nybble );

	cr.nybble = 0;
	cr.lt = 1;
	EXPECT_EQ( 8, cr.nybble );

	jit::XenonCpuContext context( 0 );

	context.cr[0].nybble = 1;
	context.cr[1].nybble = 2;
	context.cr[2].nybble = 3;
	context.cr[3].nybble = 4;
	context.cr[4].nybble = 5;
	context.cr[5].nybble = 6;
	context.cr[6].nybble = 7;
	context.cr[7].nybble = 8;

	EXPECT_EQ( 0x12345678, context.ReadCr() );
}

