#include "xenon/XenonMachineContext.h"
#include "ElfLoader.h"

#include "jit/powerpc/XenonCpuContext.h"

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
	jit::XenonCpuContext *cpuContext = new (perThreadContext) jit::XenonCpuContext( 0 );

	LoadLibXenonExecutable( path, &(cpuContext->pc) );
	cpuContext->msr = 0;
	cpuContext->msr |= 0x8000000000000000UL;  //64bit mode
	cpuContext->msr |= 0x1000000000000000UL;  //Hypervisor mode
	cpuContext->msr |= 0x0000000002000000UL;  //Vector available
	cpuContext->msr |= 0x0000000000002000UL;  //Floating point available
	cpuContext->msr |= 0x0000000000001000UL;  //Machine check enable
}

} //namespace Gen7

