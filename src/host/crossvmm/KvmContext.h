#ifndef GEN7_HOST_CROSSVMM_KVMCONTEXT_H
#define GEN7_HOST_CROSSVMM_KVMCONTEXT_H

#include <unistd.h>

#include <cstdint>

struct kvm_run;
struct kvm_segment;

namespace Gen7 {

class CrossVmmPhysicalMemory;

class KvmContext
{
private:
	static const uint8_t INITIAL_GDT[24];

	static const uint64_t INITIAL_GDT_LOCATION = 0x1000;

	static const char * KVM_DEVICE_FILE_NAME;

	static const uint32_t TSS_LOCATION = 0xE0000000;

	CrossVmmPhysicalMemory &physMem;

	int kvmFd;
	int vmFd;
	int vcpuFd;
	int kvmRunSize;
	bool running;

	struct kvm_run *kvmRun;

private:
	void OpenKvmInterface();
	void MmapKvmRun();
	void SetupMemory();
	void SetupRegisters();
	void DumpRegisters();

	bool KvmIoctl( int request, void *value, int &ret );
	bool VmIoctl( int request, void *value, int &ret );
	bool VcpuIoctl( int request, void *value, int &ret );

	void SetSegmentFlags( struct kvm_segment *segment, uint16_t selector, 
                          uint32_t base, uint32_t limi, uint32_t flags,
                          int unusable );

	bool KvmIoctl( int request, void *value )
	{
		int ret;
		return KvmIoctl( request, value, ret );
	}

	bool VmIoctl( int request, void *value )
	{
		int ret;
		return VmIoctl( request, value, ret );
	}

	bool VcpuIoctl( int request, void *value )
	{
		int ret;
		return VcpuIoctl( request, value, ret );
	}

	void ProcessHypercall();

public:
	void Init();

	void Run();

	KvmContext( CrossVmmPhysicalMemory &physMem )
	  : physMem( physMem )
	  , kvmFd( -1 )
	  , vmFd( -1 )
	  , vcpuFd( -1 )
	  , kvmRunSize( 0 )
	  , running( false )
	  , kvmRun( nullptr )
	{ }

	virtual ~KvmContext()
	{
		if( -1 != vmFd ) {
			close( vmFd );
		}

		if( -1 != kvmFd ) {
			close( kvmFd );
		}

		if( -1 != vcpuFd ) {
			close( vcpuFd );
		}
	}
};

} //namespace Gen7

#endif //GEN7_HOST_CROSSVMM_KVMCONTEXT_H

