#ifndef GEN7_HOST_PCIDEVICE_H
#define GEN7_HOST_PCIDEVICE_H

#include "sys/Exception.h"

#include "util/BitOps.h"

#include <cstdint>

namespace gen7 {

template<bool SwapConfig>
class PciDevice
{
private:
	const char * name;

	const uint16_t vendorId;
	const uint16_t deviceId;

public:
	uint32_t ReadConfig32( int offset ) {
		uint32_t ret;

		switch( offset ) {
			case 0: {
				ret = (deviceId << 16) | vendorId;
				break;
			}

			default: {
				throw sys::Exception( "" );
			}
		}

		if( SwapConfig ) {
			ret = util::ByteSwap<uint32_t>( ret );
		}

		return ret;
	}

	PciDevice( const char *name, uint16_t vendorId, uint16_t deviceId )
	  : name( name )
	  , vendorId( vendorId )
	  , deviceId( deviceId )
	{ }
};

} //namespace gen7

#endif //GEN7_HOST_PCIDEVICE_H

