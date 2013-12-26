#ifndef GEN7_HOST_XENON_XENONMACHINECONTEXT_H
#define GEN7_HOST_XENON_XENONMACHINECONTEXT_H

#include "xenon/XenonPhysicalMemory.h"
#include "xenon/Xenos.h"
#include "MachineContext.h"

namespace gen7 {

class XenonMachineContext : public MachineContext
{
private:
	Xenos xenos;

	XenonPhysicalMemory xenonPhysMem;

	void LoadLibXenonExecutable( const char *exePath, uint64_t *entry );

public:
	virtual void Load( const char *path );

	XenonMachineContext()
	  : MachineContext( xenonPhysMem )
	  , xenonPhysMem( *this, xenos )
	{ }
};

} //namspace gen7

#endif //GEN7_HOST_XENON_XENONMACHINECONTEXT_H

