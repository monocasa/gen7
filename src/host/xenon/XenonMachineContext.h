#ifndef GEN7_HOST_XENON_XENONMACHINECONTEXT_H
#define GEN7_HOST_XENON_XENONMACHINECONTEXT_H

#include "xenon/XenonPhysicalMemory.h"
#include "MachineContext.h"

namespace Gen7 {

class XenonMachineContext : public MachineContext
{
private:
	XenonPhysicalMemory xenonPhysMem;

	void LoadLibXenonExecutable( const char *exePath, uint64_t *entry );

public:
	virtual void Load( const char *path );

	XenonMachineContext()
	  : MachineContext( xenonPhysMem )
	  , xenonPhysMem( *this )
	{ }
};

} //namspace Gen7

#endif //GEN7_HOST_XENON_XENONMACHINECONTEXT_H

