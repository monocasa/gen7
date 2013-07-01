#ifndef GEN7_HOST_DC_DCMACHINECONTEXT_H
#define GEN7_HOST_DC_DCMACHINECONTEXT_H

#include "dc/DcPhysicalMemory.h"
#include "MachineContext.h"

namespace Gen7 {

class DcMachineContext : public MachineContext
{
private:
	DcPhysicalMemory dcPhysMem;

public:
	virtual void Load( const char */*path*/ ) { }

	DcMachineContext()
	  : MachineContext( xenonPhysMem )
	  , dcPhysMem( *this )
	{ }
};

} //namspace Gen7

#endif //GEN7_HOST_DC_DCMACHINECONTEXT_H

