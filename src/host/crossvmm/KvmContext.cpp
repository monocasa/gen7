#include "crossvmm/CrossVmmPhysicalMemory.h"
#include "crossvmm/KvmContext.h"
#include "sys/Exception.h"

#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

namespace gen7 {

const uint8_t KvmContext::INITIAL_GDT[24] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9B, 0xCF, 0x00,
	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x93, 0xCF, 0x00,
};

const char * KvmContext::KVM_DEVICE_FILE_NAME = "/dev/kvm";

void KvmContext::OpenKvmInterface()
{
	if( (kvmFd = open( KVM_DEVICE_FILE_NAME, O_RDWR )) < 0 ) {
		throw Sys::Exception( "Unable to open %s", KVM_DEVICE_FILE_NAME );
	}
}

void KvmContext::MmapKvmRun()
{
	void * addr = mmap( nullptr, kvmRunSize, PROT_READ | PROT_WRITE, MAP_SHARED, vcpuFd, 0 );

	if( MAP_FAILED == addr ) {
		throw Sys::Exception( "Unable to mmap kvm_run" );
	}

	kvmRun = reinterpret_cast<struct kvm_run*>( addr );
}

void KvmContext::SetupMemory()
{
	int slotNum = 0;
	for( const auto& entry : physMem.GetMemoryEntries() ) {
		uint64_t vmmLocation = (entry.memRegion * 0x100000000UL) + entry.regionOffset;

		struct kvm_userspace_memory_region region;

		region.slot = slotNum;
		region.flags = 0;
		region.guest_phys_addr = vmmLocation;
		region.memory_size = entry.size;
		region.userspace_addr = reinterpret_cast<unsigned long>( entry.addr );

		if( !VmIoctl( KVM_SET_USER_MEMORY_REGION, &region ) ) {
			throw Sys::Exception( "Unable to map region %s(slot=%d, vmmLocation=0x%016lx, hostLocation=%p)", entry.name, slotNum, vmmLocation, entry.addr );
		}

		slotNum++;
	}
}

void KvmContext::SetupRegisters()
{
	struct kvm_regs regs;
	struct kvm_sregs sregs;

	if( !VcpuIoctl( KVM_GET_REGS, &regs ) ) {
		throw Sys::Exception( "Couldn't get regs" );
	}

	if( !VcpuIoctl( KVM_GET_SREGS, &sregs ) ) {
		throw Sys::Exception( "Couldn't get sregs" );
	}

	regs.rax = 0x00000010;
	regs.rbx = 0x00000000;
	regs.rcx = 0x00000000;
	regs.rdx = 0x00000633;
	regs.rsi = 0x00000000;
	regs.rdi = 0x00000000;
	regs.rsp = 0x00003000 - 16;
	regs.rbp = 0x00000000;

	regs.rip = 0x00100000;
	regs.rflags = 0x00000046;

	sregs.cr0 = 0x60000011;
	sregs.cr2 = 0x00000000;
	sregs.cr3 = 0x00000000;
	sregs.cr4 = 0x00000000;
	sregs.efer = 0x00000000;
	sregs.apic_base = 0xFEE00000;

	SetSegmentFlags(  &sregs.cs, 0x0008, 0x00000000, 0xFFFFFFFF, 0x00C09B00, 0 );
	SetSegmentFlags(  &sregs.ds, 0x0010, 0x00000000, 0xFFFFFFFF, 0x00C09300, 0 );
	SetSegmentFlags(  &sregs.es, 0x0010, 0x00000000, 0xFFFFFFFF, 0x00C09300, 0 );
	SetSegmentFlags(  &sregs.fs, 0x0000, 0x00000000, 0x0000FFFF, 0x00009300, 0 );
	SetSegmentFlags(  &sregs.gs, 0x0000, 0x00000000, 0x0000FFFF, 0x00009300, 0 );
	SetSegmentFlags(  &sregs.ss, 0x0010, 0x00000000, 0xFFFFFFFF, 0x00C09300, 0 );
	SetSegmentFlags( &sregs.ldt, 0x0000, 0x00000000, 0x0000FFFF, 0x00008200, 0 );
	SetSegmentFlags(  &sregs.tr, 0x0000, 0x00000000, 0x0000FFFF, 0x00008B00, 0 );

	sregs.gdt.base = INITIAL_GDT_LOCATION;
	sregs.gdt.limit = sizeof(INITIAL_GDT);

	sregs.idt.base = INITIAL_GDT_LOCATION;
	sregs.idt.limit = sizeof(INITIAL_GDT);

	if( !VcpuIoctl( KVM_SET_SREGS, &sregs ) ) {
		throw Sys::Exception( "Can not set sregs" );
	}

	if( !VcpuIoctl( KVM_SET_REGS, &regs ) ) {
		throw Sys::Exception( "Can no set regs" );
	}
}

void KvmContext::DumpRegisters()
{
	struct kvm_regs regs;

	if( !VcpuIoctl( KVM_GET_REGS, &regs ) ) {
		throw Sys::Exception( "Couldn't get regs" );
	}

	printf( "   rax %016llx | rbx %016llx | rcx %016llx | rdx %016llx\n", regs.rax, regs.rbx, regs.rcx, regs.rdx );
	printf( "   rdi %016llx | rsi %016llx | rsp %016llx | rbp %016llx\n", regs.rdi, regs.rsi, regs.rsp, regs.rbp );
	printf( "    r8 %016llx |  r9 %016llx | r10 %016llx | r11 %016llx\n",  regs.r8,  regs.r9, regs.r10, regs.r11 );
	printf( "   r12 %016llx | r13 %016llx | r14 %016llx | r15 %016llx\n", regs.r12, regs.r13, regs.r14, regs.r15 );
	printf( "rflags %016llx | rip %016llx\n", regs.rflags, regs.rip );
}

bool KvmContext::KvmIoctl( int request, void *value, int &ret )
{
	if( -1 == kvmFd ) {
		throw Sys::Exception( "KvmIoctl when kvmFd not open" );
	}

	if( (ret = ioctl( kvmFd, request, value )) < 0 ) {
		return false;
	}

	return true;
}

bool KvmContext::VmIoctl( int request, void *value, int &ret )
{
	if( -1 == vmFd ) {
		throw Sys::Exception( "VmIoctl when vmFd not open" );
	}

	if( (ret = ioctl( vmFd, request, value )) < 0 ) {
		return false;
	}

	return true;
}

bool KvmContext::VcpuIoctl( int request, void *value, int &ret )
{
	if( -1 == vcpuFd ) {
		throw Sys::Exception( "VcpuIoctl when vmFd not open" );
	}

	if( (ret = ioctl( vcpuFd, request, value )) < 0 ) {
		return false;
	}

	return true;
}

void KvmContext::SetSegmentFlags( struct kvm_segment *segment, uint16_t selector, 
                                   uint32_t base, uint32_t limit, uint32_t flags,
                                   int unusable )
{
	segment->selector = selector;

	segment->base = base;

	segment->limit = limit;

	segment->type    = (flags >> 8) & 0xf;
	segment->present = (flags >> 15) & 1;
	segment->dpl     = (flags >> 13) & 3;
	segment->db      = (flags >> 22) & 1;
	segment->s       = (flags >> 12) & 1;
	segment->l       = (flags >> 21) & 1;
	segment->g       = (flags >> 23) & 1;
	segment->avl     = (flags >> 20) & 1;

	segment->unusable = unusable;
}

void KvmContext::ProcessHypercall()
{
	struct kvm_regs regs;

	if( !VcpuIoctl( KVM_GET_REGS, &regs ) ) {
		throw Sys::Exception( "Couldn't get regs" );
	}

	switch( regs.rdi ) {
		case 0: {
			printf( "hyper_quit called\n" );
			running = false;
			break;
		}

		case 1: {
			printf( "%c", (char)regs.rsi );
			regs.rax = 0;
			break;
		}
	}


	if( !VcpuIoctl( KVM_SET_REGS, &regs ) ) {
		throw Sys::Exception( "Couldn't set regs" );
	}
}

void KvmContext::Init()
{
	OpenKvmInterface();

	if( !KvmIoctl( KVM_CREATE_VM, nullptr, vmFd ) ) {
		throw Sys::Exception( "Error creating VM" );
	}

	if( !VmIoctl( KVM_SET_TSS_ADDR, (void*)TSS_LOCATION ) ) {
		throw Sys::Exception( "Error setting TSS address range" );
	}

	if( !VmIoctl( KVM_CREATE_VCPU, nullptr, vcpuFd ) ) {
		throw Sys::Exception( "Error creating vcpu" );
	}

	if( !KvmIoctl( KVM_GET_VCPU_MMAP_SIZE, nullptr, kvmRunSize ) ) {
		throw Sys::Exception( "Error getting kvm_run size" );
	}

	MmapKvmRun();

	for( size_t i = 0; i < sizeof(INITIAL_GDT); i++ ) {
		physMem.WritePhys8( INITIAL_GDT_LOCATION + i, INITIAL_GDT[i] );
	}

	SetupMemory();
	SetupRegisters();
}

void KvmContext::Run()
{
	running = true;

	while( running ) {
		if( !VcpuIoctl( KVM_RUN, nullptr ) ) {
			throw Sys::Exception( "Can't KVM_RUN (%d)", kvmRun->exit_reason );
		}

		switch( kvmRun->exit_reason ) {
			case KVM_EXIT_IO: {
				ProcessHypercall();
				break;
			}

			case KVM_EXIT_MMIO: {
				int len = kvmRun->mmio.len;
				uint64_t addr = kvmRun->mmio.phys_addr;
				uint16_t* data16 = reinterpret_cast<uint16_t*>( kvmRun->mmio.data );
				uint32_t* data32 = reinterpret_cast<uint32_t*>( kvmRun->mmio.data );

				if( kvmRun->mmio.is_write ) {
					switch( len ) {
						case 2: {
							physMem.WritePhys16( addr, *data16 );
							break;
						}

						case 4: {
							physMem.WritePhys32( addr, *data32 );
							break;
						}
						default: {
							throw Sys::Exception( "Implment mmio write for len %d (data = %08x)\n", len, *data32 );
						}
					}
				}
				else {
					switch( len ) {
						case 4: {
							*data32 = physMem.ReadPhys32( addr );
							break;
						}
						default: {
							throw Sys::Exception( "Implement mmio read for len %d", len );
						}
					}
				}
				break;
			}

			case KVM_EXIT_FAIL_ENTRY: {
				throw Sys::Exception( "KVM_EXIT_FAIL_ENTRY -> 0x%08x", kvmRun->hw.hardware_exit_reason );
			}

			case KVM_EXIT_INTERNAL_ERROR: {
				throw Sys::Exception( "KVM_EXIT_INTERNAL_ERROR:  %d", kvmRun->internal.suberror );
			}

			default: {
				DumpRegisters();
				throw Sys::Exception( "Unknown KVM_RUN exit reason %d", kvmRun->exit_reason );
			}
		}
	}
}			

} //namespace gen7

