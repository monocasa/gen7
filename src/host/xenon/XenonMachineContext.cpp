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
	XenonPpcContext *ppcContext = crossVmm.GetPerThreadBase<XenonPpcContext>();

	std::memset( ppcContext, 0, sizeof(XenonPpcContext) );

	LoadLibXenonExecutable( path, &(ppcContext->pc) );
}

} //namespace Gen7

