#ifndef GEN7_CROSSVMM_VMM_INTERRUPTMANAGER_H
#define GEN7_CROSSVMM_VMM_INTERRUPTMANAGER_H

#include <cstdint>

struct InterruptRegs {
	uint64_t r15, r14, r13, r12;
	uint64_t r11, r10,  r9,  r8;
	uint64_t rbp, rdi, rsi;
	uint64_t rdx, rcx, rbx, rax;
	uint64_t intNum;
	uint32_t errorCode;
	uint32_t rsvd;
	uint64_t rip;
	uint8_t  cs;
	uint8_t  cs_rsvd[7];
	uint64_t rflags;
	uint64_t rsp;
	uint8_t  ss;
	uint8_t  ss_rsvd[7];
};

class InterruptManager
{
private:
	static const int NUM_IDT_ENTRIES = 256;

	static const int BREAKPOINT_ISR_NUM = 3;
	static const int PAGEFAULT_ISR_NUM = 14;

	struct Idtr {
		uint16_t Limit;
		uint64_t Base;
	} __attribute__((__packed__)) idtr;

	struct IdtEntry {
		static const uint16_t PRESENT_FLAG = 0x8000;
		static const uint16_t INTERRUPT_GATE_FLAG = 0x0E00;

		uint16_t offset_15_0;
		uint16_t selector;
		uint16_t flags;
		uint16_t offset_31_16;
		uint32_t offset_63_32;
		uint32_t rsvd;

		void Clear();
		void Set( void(*isr)() );
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

