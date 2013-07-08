#include "xenon/XenonMachineContext.h"
#include "ElfLoader.h"

#include "shared/Types.h"

#include <cstring>

namespace Gen7 {

void XenonMachineContext::LoadLibXenonExecutable( const char * exePath, uint64_t* entry )
{
	ElfLoader elfLoader( exePath, ElfLoader::POWERPC32 );
	elfLoader.Load( xenonPhysMem );

	*entry =  elfLoader.GetEntry();
}

void XenonMachineContext::Load( const char *path )
{
	void * perThreadContext = crossVmm.GetPerThreadBase();
	XenonPpcContext *ppcContext = new (perThreadContext) XenonPpcContext( 0 );

	LoadLibXenonExecutable( path, &(ppcContext->pc) );
}

} //namespace Gen7

