#include "crossvmm/CrossVmm.h"
#include "sys/Exception.h"
#include "MachineContext.h"

#include <cstdio>

void Usage()
{
	fprintf( stderr, "\tgen7-cli XELL_EXECUTABLE\n" );
}

void PrintBanner()
{
	printf( "====================================================\n" );
	printf( "  .oooooo.    oooooooooooo ooooo      ooo  ooooooooo\n" );
	printf( " d8P'  `Y8b   `888'     `8 `888b.     `8' d\"\"\"\"\"\"\"8'\n" );
	printf( "888            888          8 `88b.    8        .8'\n" );
	printf( "888            888oooo8     8   `88b.  8       .8'\n" );
	printf( "888     ooooo  888    \"     8     `88b.8      .8'\n" );
	printf( "`88.    .88'   888       o  8       `888     .8'\n" );
	printf( " `Y8bood8P'   o888ooooood8 o8o        `8    .8'\n" );
	printf( "====================================================\n" );
	printf( "          A Generation 7 Low Level Emulator\n" );
	printf( "====================================================\n" );
}

int main( int argc, char* argv[] )
{
	if( argc != 2 ) {
		Usage();
		return 1;
	}

	const char* exePath = argv[1];

	PrintBanner();

	Gen7::MachineContext context( Gen7::ConsoleType::RETAIL );

	try {
		context.Init();
	} catch( Sys::Exception& ex ) {
		printf( "=============================\n" );
		printf( "Caught exception during init:\n" );
		printf( "\n" );
		printf( "%s\n", ex.what() );
		printf( "\n" );
		printf( "Backtrace:\n" );
		for( int i = 0; i < ex.GetNumSymbols(); i++ ) {
			printf( "\t%s\n", ex.GetSymbol( i ) );
		}

		return 1;
	}

	try {
		context.LoadLibXenonExecutable( exePath );
		context.Run();
	} catch( Sys::Exception& ex ) {
		printf( "=====================================================\n" );
		printf( "Caught exception while executing libxenon executable:\n" );
		printf( "\n" );
		printf( "%s\n", ex.what() );
		printf( "\n" );
		printf( "Backtrace:\n" );
		for( int i = 0; i < ex.GetNumSymbols(); i++ ) {
			printf( "\t%s\n", ex.GetSymbol( i ) );
		}

		context.DumpState();

		return 1;
	}

	return 0;
}

