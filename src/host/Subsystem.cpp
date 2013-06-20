#include "MachineContext.h"
#include "Subsystem.h"

namespace Gen7 {

Subsystem::Subsystem( MachineContext &context )
  : context( context )
{
	context.RegisterSubsystem( this );
}

} //namespace Gen7

