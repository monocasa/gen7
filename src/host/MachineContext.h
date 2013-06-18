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
	const ConsoleType consoleType;

	XenonPhysicalMemory physMem;

	CrossVmm crossVmm;

	std::vector<Subsystem*> subsystems;

public:
	void Init();
	void Run();
	void DumpState();

	void LoadLibXenonExecutable( const char *exePath );

	ConsoleType GetConsoleType() const
	{
		return consoleType;
	}

	MachineContext( ConsoleType consoleType )
	  : consoleType( consoleType )
	  , physMem( *this )
	  , crossVmm( *this )
	{
		subsystems.push_back( &physMem );
		subsystems.push_back( &crossVmm );
	}
};

} //namespace Gen7

#endif //GEN7_HOST_MACHINECONTEXT_H

