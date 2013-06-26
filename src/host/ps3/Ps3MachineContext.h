#ifndef GEN7_HOST_PS3_PS3MACHINECONTEXT_H
#define GEN7_HOST_PS3_PS3MACHINECONTEXT_H

#include "ps3/Ps3PhysicalMemory.h"
#include "MachineContext.h"

namespace Gen7 {

class Ps3MachineContext : public MachineContext
{
private:
	Ps3PhysicalMemory ps3PhysMem;

public:
	Ps3MachineContext()
	  : MachineContext( ps3PhysMem )
	  , ps3PhysMem( *this )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_PS3_PS3MACHINECONEXT_H

