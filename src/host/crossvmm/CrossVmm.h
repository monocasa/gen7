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
		return "CrossVmm";
	}

	virtual const char * GetShortName() const {
		return "CRSVMM";
	}

	CrossVmm( MachineContext &context, NativePhysicalMemory &nativeMem )
	  : Subsystem( context )
	  , physMem( context, nativeMem )
	  , kvmContext( physMem )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_CROSSVMM_CROSSVMM_H

