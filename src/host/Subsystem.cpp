#include "MachineContext.h"
#include "Subsystem.h"

namespace gen7 {

Subsystem::Subsystem( MachineContext &context )
  : context( context )
{
	context.RegisterSubsystem( this );
}

} //namespace gen7

