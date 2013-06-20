#ifndef GEN7_HOST_MACHINECONTEXT_H
#define GEN7_HOST_MACHINECONTEXT_H

#include "crossvmm/CrossVmm.h"
#include "InterpCpu.h"
#include "XenonPhysicalMemory.h"

#include "shared/Types.h"

#include <vector>

namespace Gen7 {

class MachineContext
{
private:
	std::vector<Subsystem*> subsystems;

	const ConsoleType consoleType;

	XenonPhysicalMemory physMem;

	CrossVmm crossVmm;

public:
	void Init();
	void Run();
	void DumpState();

	void LoadLibXenonExecutable( const char *exePath );

	void RegisterSubsystem( Subsystem* subsystem );

	ConsoleType GetConsoleType() const
	{
		return consoleType;
	}

	MachineContext( ConsoleType consoleType )
	  : consoleType( consoleType )
	  , physMem( *this )
	  , crossVmm( *this, physMem )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_MACHINECONTEXT_H

