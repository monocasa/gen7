#ifndef GEN7_HOST_MACHINECONTEXT_H
#define GEN7_HOST_MACHINECONTEXT_H

#include "crossvmm/CrossVmm.h"
#include "InterpCpu.h"
#include "PhysicalMemory.h"

#include <vector>

namespace Gen7 {

class MachineContext
{
private:
	std::vector<Subsystem*> subsystems;

	NativePhysicalMemory &physMem;

	CrossVmm crossVmm;

public:
	void Init();
	void Run();
	void DumpState();

	void RegisterSubsystem( Subsystem* subsystem );

	MachineContext( NativePhysicalMemory &physMem )
	  : physMem( physMem )
	  , crossVmm( *this, physMem )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_MACHINECONTEXT_H

