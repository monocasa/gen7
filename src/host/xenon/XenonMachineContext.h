#ifndef GEN7_HOST_XENON_XENONMACHINECONTEXT_H
#define GEN7_HOST_XENON_XENONMACHINECONTEXT_H

#include "xenon/XenonPhysicalMemory.h"
#include "MachineContext.h"

namespace Gen7 {

class XenonMachineContext : public MachineContext
{
private:
	XenonPhysicalMemory xenonPhysMem;

public:
	void LoadLibXenonExecutable( const char *exePath );

	XenonMachineContext()
	  : MachineContext( xenonPhysMem )
	  , xenonPhysMem( *this )
	{ }
};

} //namspace Gen7

#endif //GEN7_HOST_XENON_XENONMACHINECONTEXT_H

