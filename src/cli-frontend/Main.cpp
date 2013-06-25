#include "crossvmm/CrossVmm.h"
#include "sys/Exception.h"
#include "xenon/XenonMachineContext.h"
#include "MachineContext.h"

#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>

#include <cstdio>
#include <iostream>

namespace po = boost::program_options;

const char * banner = 
	"====================================================\n"
	"  .oooooo.    oooooooooooo ooooo      ooo  ooooooooo\n"
	" d8P'  `Y8b   `888'     `8 `888b.     `8' d\"\"\"\"\"\"\"8'\n"
	"888            888          8 `88b.    8        .8'\n"
	"888            888oooo8     8   `88b.  8       .8'\n"
	"888     ooooo  888    \"     8     `88b.8      .8'\n"
	"`88.    .88'   888       o  8       `888     .8'\n"
	" `Y8bood8P'   o888ooooood8 o8o        `8    .8'\n"
	"====================================================\n"
	"          A Generation 7 Low Level Emulator\n"
	"====================================================\n";

enum Platform
{
	UNSPECIFIED,
	XENON,
	PS3,
};

Platform platform;
bool showBanner = true;

void PrintMachineList()
{
	std::cerr << "Valid Machines:" << std::endl;
	std::cerr << "\txenon - Xbox360" << std::endl;
	std::cerr << "\tps3   - PlayStation3" << std::endl;
}

std::istream& operator>>( std::istream &in, Platform &platform )
{
	std::string token;

	in >> token;

	if( "?" == token ) {
		PrintMachineList();
		exit( 1 );
	}
	else if( "xenon" == token ) {
		platform = XENON;
	}
	else {
		throw po::validation_error( po::validation_error::invalid_option_value, token, "m" );
	}

	return in;
}

void PrintBanner()
{
	printf( "%s", banner );
}

void ParseArgs( int argc, char *argv[] )
{
	po::variables_map vm;

	po::options_description desc( "Options" );
	desc.add_options()
		( "help",        "Print this help message" )
		( "no-banner,n", "Don't display a banner" )
		( "machine,m", po::value<Platform>( &platform )->required(), 
		               "The machine to emulate (? to list)" );

	try {

		po::store( po::parse_command_line( argc, argv, desc ), vm );

		po::notify( vm );
	}
	catch( const po::error &exc ) {
		std::cerr << "Options parse error:" << std::endl;
		std::cerr << "\t" << exc.what() << std::endl;
		exit( 1 );
	}

	if( vm.count( "help" ) ) {
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " -m MACHINE [OPTIONS] [EXEFILE]"<< std::endl;
		std::cerr << std::endl;
		std::cerr << desc << std::endl;
		exit( 1 );
	}

	if( vm.count( "no-banner" ) ) {
		showBanner = false;
	}
}

int main( int argc, char* argv[] )
{
	ParseArgs( argc, argv );
	Gen7::MachineContext *context = nullptr;

	if( showBanner ) {
		PrintBanner();
	}

	try {
		switch( platform ) {
			case XENON: {
				context = new Gen7::XenonMachineContext();
				break;
			}

			case PS3: {
				break;
			}

			default:
			case UNSPECIFIED: {
				std::cerr << "ERROR:\t" << std::endl << "No platform specified" << std::endl;
				return 1;
			}
		}

		context->Init();
		context->Run();

	} catch( Sys::Exception& ex ) {
		printf( "==================================\n" );
		printf( "Caught exception during execution:\n" );
		printf( "\n" );
		printf( "%s\n", ex.what() );
		printf( "\n" );
		printf( "Backtrace:\n" );
		for( int i = 0; i < ex.GetNumSymbols(); i++ ) {
			printf( "\t%s\n", ex.GetSymbol( i ) );
		}

		return 1;
	}

	return 0;
}

