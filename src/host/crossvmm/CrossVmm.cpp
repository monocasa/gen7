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
		case InitPhase::ALLOCATION: {
			physMem.Init();

			ElfLoader elfLoader( "crossvmm.bin", ElfLoader::X86_64 );
			elfLoader.Load( physMem );

			break;
		}

		case InitPhase::APP_1: {
			kvmContext.Init(); // dependent on ALLOCATION
			break;
		}

		default: {
			break;
		}
	}
}

} //namespace Gen7

