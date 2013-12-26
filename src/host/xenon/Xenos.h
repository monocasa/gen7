#ifndef GEN7_HOST_XENON_XENOS_H
#define GEN7_HOST_XENON_XENOS_H

#include "PciDevice.h"

namespace gen7 {

class Xenos : public PciDevice<true>
{
public:
	Xenos()
	  : PciDevice( "xenos", 0x1414, 0x5841 )
	{ }
};

} //namespace gen7

#endif //GEN7_HOST_XENON_XENOS_H

