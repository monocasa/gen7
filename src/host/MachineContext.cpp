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

void MachineContext::RegisterSubsystem( Subsystem *subsystem )
{
	printf( "Registering %p\n", subsystem );

	subsystems.push_back( subsystem );
}

} //namespace Gen7

