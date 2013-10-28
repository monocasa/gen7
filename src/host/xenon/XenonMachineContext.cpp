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
	ppcContext->msr = 0;
	ppcContext->msr |= 0x8000000000000000UL;  //64bit mode
	ppcContext->msr |= 0x1000000000000000UL;  //Hypervisor mode
	ppcContext->msr |= 0x0000000002000000UL;  //Vector available
	ppcContext->msr |= 0x0000000000002000UL;  //Floating point available
	ppcContext->msr |= 0x0000000000001000UL;  //Machine check enable
}

} //namespace Gen7

