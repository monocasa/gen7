#include "dc/DcMachineContext.h"
#include "ElfLoader.h"

#include "shared/Types.h"

#include <cstring>

namespace Gen7 {

void DcMachineContext::LoadElf( const char *exePath, Sh4aContext &context )
{
	ElfLoader elfLoader( exePath, ElfLoader::POWERPC32 );
	elfLoader.Load( machineMemory );

	context.pc      = elfLoader.GetEntry();
	context.sr      = 0x700000F0;
	context.gpr[15] = 0x8D000000;
}

void DcMachineContext::LoadBios( Sh4aContext &context )
{
	context.pc     = 0xA0000000;
	context.sr     = 0x700000F0;
	context.expevt = 0x00000020;
	context.vbr    = 0x00000000;
}

void DcMachineContext::Load( const char *path )
{
	void * perThreadContext = crossVmm.GetPerThreadBase();
	Sh4aContext *context = new (perThreadContext) Sh4aContext();

	if( strlen( path ) != 0 ) {
		LoadElf( path, *context );
	}
	else {
		LoadBios( *context );
	}
}

} //namespace Gen7

