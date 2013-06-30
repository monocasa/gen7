#ifndef GEN7_CROSSVMM_VMM_MEMORYMANAGER_H
#define GEN7_CROSSVMM_VMM_MEMORYMANAGER_H

#include <cstdint>

class MemoryManager
{
public:
	struct Page {
		uint8_t data[4096];
	};

private:
	static const int KIB = 1024;
	static const int MIB = 1024 * KIB;
	static const int GIB = 1024 * MIB;

	static const uint64_t VMM_PAGE_SIZE =   2 * MIB;
	static const uint64_t VMM_WRAM_SIZE = 512 * MIB;
	static const int NUM_WRAM_PAGES = VMM_WRAM_SIZE / VMM_PAGE_SIZE;

	static const uint64_t VMM_WRAM_VIRT_BASE = 0xFFFFFFFF80000000UL;

	//Top level page table
	// Full 256TiB view of 512 512GiB PML3s
	uint64_t *pml4;

	//Middle page table for top of memory (shared between cores, should be static)
	// 512GiB view of 512 1GiB PML2s
	uint64_t *higherPml3;

	//Bottom level page table for VMM Work RAM (shared between cores, should be static)
	// 1GiB view of 512 2MiB PML1s
	uint64_t *vmmPml2;

	struct HeapPageInfo {
		uint8_t allocated : 1;
		uint8_t RSVD      : 7;
	};

	Page *contiguousHeap;
	uint64_t contiguousHeapSize;
	uint64_t numContiguousPages;

	HeapPageInfo *contiguousHeapInfo;
	uint64_t numContiguousHeapInfoPages;

	void* VmmVirtToPhys( void *phys );
	void* PhysToVmmVirt( void *phys );

	void PrintPage( uint64_t *page );
	void InvalidatePage( uint64_t addr );
	void InvalidateAllPages();
	void MapVmmWram();
	void UnmapBootLower();
	void InitializeContiguousHeap();

public:
	Page* AllocatePage();

	void SetPml2Page( uint64_t *page, uint64_t addr );

	void SetLowerPml3( uint64_t *page, uint64_t addr );

	void Init();
};

extern MemoryManager mm;

#endif //GEN7_CROSSVMM_VMM_MEMORYMANAGER_H

