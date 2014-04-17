#include "sys/Exception.h"
#include "wii/WiiMachineContext.h"
#include "ElfLoader.h"

#include <cstdio>
#include <cstring>

namespace gen7 {

uint64_t WiiMachineContext::WiiDefaultVirtualMem::getPhysForVirt( uint64_t virt ) {
	switch( virt >> 28 ) {
		case 0x8 ... 0x9: {
			return virt - 0x80000000;
		}

		case 0xA ... 0xB: {
			return virt - 0xA0000000;
		}

		default: {
			throw sys::Exception( "Unmapped Wii virt address 0x%lx", virt );
		}
	}
}

void WiiMachineContext::WiiDefaultVirtualMem::WritePhys8( uint64_t addr, uint8_t data )
{
	uint64_t physAddr = getPhysForVirt( addr );

	physMem.WritePhys8( physAddr, data );
}

void WiiMachineContext::WiiDefaultVirtualMem::WritePhys16( uint64_t addr, uint16_t data )
{
	uint64_t physAddr = getPhysForVirt( addr );

	physMem.WritePhys16( physAddr, data );
}

void WiiMachineContext::WiiDefaultVirtualMem::WritePhys32( uint64_t addr, uint32_t data )
{
	uint64_t physAddr = getPhysForVirt( addr );

	physMem.WritePhys32( physAddr, data );
}

uint32_t WiiMachineContext::WiiDefaultVirtualMem::ReadPhys32( uint64_t addr )
{
	uint64_t physAddr = getPhysForVirt( addr );

	return physMem.ReadPhys32( physAddr );
}

void WiiMachineContext::WiiDefaultVirtualMem::WritePhys64( uint64_t addr, uint64_t data )
{
	uint64_t physAddr = getPhysForVirt( addr );

	physMem.WritePhys64( physAddr, data );
}

void WiiMachineContext::Load( const char *path )
{
	void * perThreadContext = crossVmm.GetPerThreadBase();
	jitpp::GekkoCpuContext *context = new (perThreadContext) jitpp::GekkoCpuContext();

	if( strlen(path) == 0 ) {
		throw sys::Exception( "You must load a file in Wii emulation" );
	}

	ElfLoader elfLoader( path, ElfLoader::POWERPC32 );
	elfLoader.Load( defaultVirtMem );

	context->pc = elfLoader.GetEntry();
}

} //namespace gen7

