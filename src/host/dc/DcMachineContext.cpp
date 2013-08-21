#include "dc/DcMachineContext.h"

#include "shared/Types.h"

#include <cstring>

namespace Gen7 {

void DcMachineContext::LoadBios( Sh4aContext &context )
{
	context.pc     = 0xA0000000;
	context.expevt = 0x00000020;
	context.vbr    = 0x00000000;
}

void DcMachineContext::Load( const char *path )
{
	void * perThreadContext = crossVmm.GetPerThreadBase();
	Sh4aContext *context = new (perThreadContext) Sh4aContext();

	if( strlen( path ) != 0 ) {
		throw Sys::Exception( "Dreamcast does not yet support --exec" );
	}

	LoadBios( *context );
}

} //namespace Gen7

