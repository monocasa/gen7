#ifndef GEN7_CROSSVMM_VMM_INTERRUPTMANAGER_H
#define GEN7_CROSSVMM_VMM_INTERRUPTMANAGER_H

#include <cstdint>

class InterruptManager
{
private:
	static const int NUM_IDT_ENTRIES = 256;

	struct Idtr {
		uint16_t Limit;
		uint64_t Base;
	} __attribute__((__packed__)) idtr;

	struct IdtEntry {
		uint16_t offset_15_0;
		uint16_t selector;
		uint16_t flags;
		uint16_t offset_31_16;
		uint32_t offset_63_32;
		uint32_t rsvd;
	} __attribute((__packed__));

	IdtEntry* idtEntries;

	void PopulateIdtEntries();
	void InstallIdt();

	void DisableInterrupts();
	void EnableInterrupts();

public:
	void Init();
};

extern InterruptManager intm;

#endif //GEN7_CROSSVMM_VMM_INTERRUPTMANAGER_H

