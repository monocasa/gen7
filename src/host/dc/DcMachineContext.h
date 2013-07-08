#ifndef GEN7_HOST_DC_DCMACHINECONTEXT_H
#define GEN7_HOST_DC_DCMACHINECONTEXT_H

#include "dc/DcPhysicalMemory.h"
#include "MachineContext.h"

namespace Gen7 {

struct Sh4aContext;

class DcMachineContext : public MachineContext
{
private:
	DcPhysicalMemory dcPhysMem;

	void LoadBios( Sh4aContext &context );

public:
	virtual void Load( const char *path );

	DcMachineContext()
	  : MachineContext( dcPhysMem )
	  , dcPhysMem( *this )
	{ }
};

} //namspace Gen7

#endif //GEN7_HOST_DC_DCMACHINECONTEXT_H

