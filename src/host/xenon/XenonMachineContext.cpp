#include "xenon/XenonMachineContext.h"
#include "ElfLoader.h"

namespace Gen7 {

void XenonMachineContext::LoadLibXenonExecutable( const char * exePath )
{
	ElfLoader elfLoader( exePath, ElfLoader::POWERPC32 );
	elfLoader.Load( xenonPhysMem );

//	cpu.Reset();
//	cpu.SetPc( elfLoader.GetEntry() );
}

} //namespace Gen7

