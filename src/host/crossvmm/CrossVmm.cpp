#include "crossvmm/CrossVmm.h"
#include "sys/File.h"
#include "ElfLoader.h"

namespace Gen7 {

void CrossVmm::Run()
{
	kvmContext.Run();
}

void CrossVmm::Init( InitPhase phase )
{
	switch( phase ) {
		case InitPhase::APP_1: {
			// dependent on CrossVmmPhysicalMemory::Init( ALLOCATION )
			ElfLoader elfLoader( "crossvmm.bin", ElfLoader::X86_64 );
			elfLoader.Load( physMem );

			break;
		}

		case InitPhase::APP_2: {
			// dependent on CrossVmmPhysicalMemory::Init( APP_1 )
			kvmContext.Init();
			break;
		}

		default: {
			break;
		}
	}
}

} //namespace Gen7

