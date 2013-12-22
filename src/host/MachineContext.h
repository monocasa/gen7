#ifndef GEN7_HOST_MACHINECONTEXT_H
#define GEN7_HOST_MACHINECONTEXT_H

#include "crossvmm/CrossVmm.h"
#include "PhysicalMemory.h"

#include <vector>

namespace gen7 {

class MachineContext
{
private:
	std::vector<Subsystem*> subsystems;

protected:
	NativePhysicalMemory &physMem;

	CrossVmm crossVmm;

public:
	void Init();
	void Run();
	void DumpState();

	virtual void Load( const char *path ) = 0;

	void RegisterSubsystem( Subsystem* subsystem );

	MachineContext( NativePhysicalMemory &physMem )
	  : physMem( physMem )
	  , crossVmm( *this, physMem )
	{ }
};

} //namespace gen7

#endif //GEN7_HOST_MACHINECONTEXT_H

