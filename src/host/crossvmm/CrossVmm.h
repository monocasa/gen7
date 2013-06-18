#ifndef GEN7_HOST_CROSSVMM_CROSSVMM_H
#define GEN7_HOST_CROSSVMM_CROSSVMM_H

#include "crossvmm/CrossVmmPhysicalMemory.h"
#include "crossvmm/KvmContext.h"
#include "Subsystem.h"

namespace Gen7 {

class CrossVmm : public Subsystem
{
private:
	CrossVmmPhysicalMemory physMem;

	KvmContext kvmContext;

public:
	void Run();

	virtual void Init( InitPhase phase );

	virtual const char * GetName() const {
		return "CrossVMM";
	}

	virtual const char * GetShortName() const {
		return "CRVM";
	}

	CrossVmm( MachineContext &context )
	  : Subsystem( context )
	  , kvmContext( physMem )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_CROSSVMM_CROSSVMM_H

