#ifndef GEN7_HOST_WII_WIIMACHINECONTEXT_H
#define GEN7_HOST_WII_WIIMACHINECONTEXT_H

#include "wii/WiiPhysicalMemory.h"
#include "MachineContext.h"

#include "jitpp/powerpc/GekkoCpuContext.h"

namespace gen7 {

class WiiMachineContext : public MachineContext
{
private:
	class WiiDefaultVirtualMem : public PhysicalMemory
	{
	private:
		PhysicalMemory &physMem;

		uint64_t getPhysForVirt( uint64_t virt );

	public:
		void WritePhys8( uint64_t addr, uint8_t data ) override final;
		void WritePhys16( uint64_t addr, uint16_t data ) override final;
		void WritePhys32( uint64_t addr, uint32_t data ) override final;
		uint32_t ReadPhys32( uint64_t addr ) override final;
		void WritePhys64( uint64_t addr, uint64_t data ) override final;

		WiiDefaultVirtualMem( PhysicalMemory &physMem )
		  : physMem( physMem )
		{ }
	};

	WiiPhysicalMemory wiiPhysMem;

	WiiDefaultVirtualMem defaultVirtMem;

public:
	virtual void Load( const char *path );

	WiiMachineContext()
	  : MachineContext( wiiPhysMem )
	  , wiiPhysMem( *this )
	  , defaultVirtMem( wiiPhysMem )
	{ }
};

} //namespace gen7

#endif //GEN7_HOST_WII_WIIMACHINECONTEXT_H

