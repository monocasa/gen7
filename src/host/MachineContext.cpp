#include "ElfLoader.h"
#include "Log.h"
#include "MachineContext.h"

namespace Gen7 {

void MachineContext::Init()
{
	for( int phase = (int)Subsystem::InitPhase::FIRST; phase <= (int)Subsystem::InitPhase::LAST; phase++ ) {
		for( auto subsystem : subsystems ) {
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

} //namespace Gen7

