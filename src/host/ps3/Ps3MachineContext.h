#ifndef GEN7_HOST_PS3_PS3MACHINECONTEXT_H
#define GEN7_HOST_PS3_PS3MACHINECONTEXT_H

#include "ps3/Ps3PhysicalMemory.h"
#include "sys/Exception.h"
#include "MachineContext.h"

namespace gen7 {

class Ps3MachineContext : public MachineContext
{
private:
	Ps3PhysicalMemory ps3PhysMem;

	virtual void Load( const char *path ) {
		throw Sys::Exception( "Implement Ps3MachineContext::Load( path=%s )", path );
	}

public:
	Ps3MachineContext()
	  : MachineContext( ps3PhysMem )
	  , ps3PhysMem( *this )
	{ }
};

} //namespace gen7

#endif //GEN7_HOST_PS3_PS3MACHINECONEXT_H

