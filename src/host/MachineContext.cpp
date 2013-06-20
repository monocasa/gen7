#include "ElfLoader.h"
#include "Log.h"
#include "MachineContext.h"

namespace Gen7 {

void MachineContext::Init()
{
	for( int phase = (int)Subsystem::InitPhase::FIRST; phase <= (int)Subsystem::InitPhase::LAST; phase++ ) {
		for( auto subsystem : subsystems ) {
			printf( "subsystem: %s, phase: %d\n", subsystem->GetShortName(), phase ); 
			subsystem->Init( (Subsystem::InitPhase)phase );
		}
	}
}

void MachineContext::Run()
{
	crossVmm.Run();
}

void MachineContext::DumpState()
{
//	cpu.DumpState();
}

void MachineContext::LoadLibXenonExecutable( const char * exePath )
{
	ElfLoader elfLoader( exePath, ElfLoader::POWERPC32 );
	elfLoader.Load( physMem );

//	cpu.Reset();
//	cpu.SetPc( elfLoader.GetEntry() );
}

void MachineContext::RegisterSubsystem( Subsystem *subsystem )
{
	printf( "Registering %p\n", subsystem );

	subsystems.push_back( subsystem );
}

} //namespace Gen7

