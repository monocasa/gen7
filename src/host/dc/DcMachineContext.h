#ifndef GEN7_HOST_DC_DCMACHINECONTEXT_H
#define GEN7_HOST_DC_DCMACHINECONTEXT_H

#include "dc/DcPhysicalMemory.h"
#include "MachineContext.h"

#include "jit/sh4a/Sh4aCpuContext.h"

#include <cassert>

namespace gen7 {

class DcMachineContext : public MachineContext
{
private:
	DcPhysicalMemory dcPhysMem;

	class DcMachineMemory : public PhysicalMemory {
	private:
		DcPhysicalMemory &phys;

		uint64_t ConvertToPhys( uint64_t addr ) {
			assert( (addr >= 0x80000000) && (addr <= 0xBFFFFFFF) );
			return addr & 0x1FFFFFFF;
		}

	public:
		virtual void WritePhys8( uint64_t addr, uint8_t data ) {
			phys.WritePhys8( ConvertToPhys(addr), data );
		}

		virtual void WritePhys16( uint64_t addr, uint16_t data ) {
			phys.WritePhys16( ConvertToPhys(addr), data );
		}

		virtual void WritePhys32( uint64_t addr, uint32_t data ) {
			phys.WritePhys32( ConvertToPhys(addr), data );
		}

		virtual void WritePhys64( uint64_t addr, uint64_t data ) {
			phys.WritePhys64( ConvertToPhys(addr), data );
		}

		virtual uint32_t ReadPhys32( uint64_t addr ) {
			return phys.ReadPhys32( ConvertToPhys(addr) );
		}

		DcMachineMemory( DcPhysicalMemory &phys )
		  : phys( phys )
		{ }
	} machineMemory;

	void LoadElf( const char *exePath, jit::Sh4aCpuContext &context );
	void LoadBios( jit::Sh4aCpuContext &context );

public:
	virtual void Load( const char *path );

	DcMachineContext()
	  : MachineContext( dcPhysMem )
	  , dcPhysMem( *this )
	  , machineMemory( dcPhysMem )
	{ }
};

} //namspace gen7

#endif //GEN7_HOST_DC_DCMACHINECONTEXT_H

