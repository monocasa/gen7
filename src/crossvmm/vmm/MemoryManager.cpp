#include "MemoryManager.h"

#include <cstdint>
#include <cstdio>

MemoryManager mm;

extern uint64_t __boot_pml4[];
extern uint64_t __boot_pml3[];
extern uint64_t __boot_pml2[];

extern uint8_t __bss_end[];

void* MemoryManager::VmmVirtToPhys( void *virt )
{
	uint64_t addr = reinterpret_cast<uint64_t>( virt );
	if( addr >= 0xFFFFFFFFA0000000UL ) {
		return nullptr;
	}
	else if( addr >= 0xFFFFFFFF80000000UL ) {
		return reinterpret_cast<void*>( addr & 0x7FFFFFFFUL );
	}

	return nullptr;
}

void* MemoryManager::PhysToVmmVirt( void *phys )
{
	uint64_t addr = reinterpret_cast<uint64_t>( phys );
	if( addr <= 0x7FFFFFFFUL ) {
		return reinterpret_cast<void*>(addr | 0xFFFFFFFF80000000UL );
	}

	return nullptr;
}

void MemoryManager::PrintPage( uint64_t *page )
{
	for( int i = 0; i < 0x1000; i+=8 ) {
		if( (i % 32) == 0 ) {
			printf( "%p: ", &page[i] );
		}
		printf( "%016lx ", page[i / sizeof(uint64_t)] );
		if( (i % 32) == 24 ) {
			printf( "\n" );
		}
	}
}

void MemoryManager::InvalidatePage( uint64_t addr )
{
	asm volatile( "invlpg %0"
	                :
	                : "m"(addr)
	                : "memory" );
}

void MemoryManager::InvalidateAllPages()
{
	asm volatile( "movq %cr3, %rax" );
	asm volatile( "movq %rax, %cr3" );
}

void MemoryManager::MapVmmWram()
{
	for( int i = 0; i < NUM_WRAM_PAGES; i++ ) {
		uint64_t physaddr = i * VMM_PAGE_SIZE;
		uint64_t virtaddr = physaddr | 0xFFFFFFFF80000000UL;
		uint64_t pte = physaddr | 0x83;  // is a 2MB page, w/r, preset
		vmmPml2[ i ] = pte;
		InvalidatePage( virtaddr );
	}
}

void MemoryManager::UnmapBootLower()
{
	higherPml3[0] = 0;

	pml4[0] = 0;

	InvalidateAllPages();
}

void MemoryManager::InitializeContiguousHeap()
{
	uint64_t heapEndAddr = reinterpret_cast<uint64_t>( PhysToVmmVirt( &__bss_end[0] ) );

	// Round to the next page
	heapEndAddr += 4095;
	heapEndAddr &= 0xFFFFFFFFFFFFF000UL;

	contiguousHeap = reinterpret_cast<Page*>( heapEndAddr );
	contiguousHeapSize = (VMM_WRAM_SIZE + VMM_WRAM_VIRT_BASE) - heapEndAddr;
	numContiguousPages = contiguousHeapSize / 4096;

	contiguousHeapInfo = reinterpret_cast<HeapPageInfo*>( &contiguousHeap[0] );
	numContiguousHeapInfoPages = numContiguousPages * sizeof(HeapPageInfo);
	numContiguousHeapInfoPages += 4095;
	numContiguousHeapInfoPages & 0xFFFFFFFFFFFFF000UL;
	numContiguousHeapInfoPages /= 4096;

	for( uint64_t page = 0; page < numContiguousPages; page++ ) {
		if( page < numContiguousHeapInfoPages ) {
			contiguousHeapInfo[ page ].allocated = true;
		}
		else {
			contiguousHeapInfo[ page ].allocated = false;
		}
	}

	printf( "Start of contiguous heap = %p\n", contiguousHeap );
	printf( "Heap size = 0x%08lx bytes\n", contiguousHeapSize );
}

MemoryManager::Page* MemoryManager::AllocatePage()
{
	for( int i = (numContiguousPages - 1); i > 0; i-- ) {
		if( !contiguousHeapInfo[i].allocated ) {
			contiguousHeapInfo[i].allocated = true;
			Page *page = &contiguousHeap[i];
			return page;
		}
	}

	return nullptr;
}

void MemoryManager::SetPml2Page( uint64_t *page, uint64_t virtAddr )
{
	uint64_t physPageAddr = reinterpret_cast<uint64_t>( VmmVirtToPhys( page ) );
	uint64_t offset = (virtAddr & 0x000007FFFFFFFFFUL) / (1UL * GIB);

	if( virtAddr >= 0xFFFFFF8000000000 ) {
		higherPml3[ offset ] = physPageAddr | 1;
		InvalidateAllPages();

		//PrintPage( page );
	}
	else {
		printf( "mapping %p (0x%016lx) to 0x%016lx (%03lx)\n", page, physPageAddr, virtAddr, offset );
		printf( "Implement lower\n" );
	}
}

void MemoryManager::Init()
{
	pml4 = reinterpret_cast<uint64_t*>( PhysToVmmVirt( __boot_pml4 ) );
	higherPml3 = reinterpret_cast<uint64_t*>( PhysToVmmVirt( __boot_pml3 ) );
	vmmPml2 = reinterpret_cast<uint64_t*>( PhysToVmmVirt( __boot_pml2 ) );

	// Boot only mapped bottom 8MiB, map the rest 
	MapVmmWram();

	// Unmap identity mapping used during boot
	UnmapBootLower();

	// Now that we have our RAM all mapped, carve up heap
	InitializeContiguousHeap();
}

