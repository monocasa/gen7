#include "InterruptManager.h"
#include "MemoryManager.h"
#include "System.h"

#include <cstdio>

InterruptManager intm;

extern "C" void pagefault_prologue();
extern "C" void breakpoint_prologue();

extern "C" uint64_t GetCR2();

extern "C" void FaultHandler( InterruptRegs *regs )
{
	switch( regs->intNum ) {
		case 3: {
			printf( "BREAKPOINT @ 0x%016lx\n", regs->rip - 1 );
			break;
		}

		case 14: {
			uint64_t offendingAddr = GetCR2();
			mm.OnPageFault( offendingAddr, regs );
			break;
		}

		default: {
			printf( "Unknown ISR %ld\n", regs->intNum );
			break;
		}
	}
}

void InterruptManager::IdtEntry::Clear()
{
	offset_15_0  = 0;
	offset_31_16 = 0;
	offset_63_32 = 0;
	selector = 0;
	flags = 0;
	rsvd = 0;
}

void InterruptManager::IdtEntry::Set( void(*isr)() )
{
	uint64_t isrPtr = reinterpret_cast<uint64_t>( isr );

	offset_15_0  = isrPtr;
	offset_31_16 = isrPtr >> 16;
	offset_63_32 = isrPtr >> 32;
	selector = CS_SELECTOR;
	flags = PRESENT_FLAG | INTERRUPT_GATE_FLAG;
	rsvd = 0;
}

void InterruptManager::PopulateIdtEntries()
{
	for( int i = 0; i < NUM_IDT_ENTRIES; i++ ) {
		idtEntries[i].Clear();
	}

	idtEntries[ BREAKPOINT_ISR_NUM ].Set( &breakpoint_prologue );
	idtEntries[ PAGEFAULT_ISR_NUM ].Set( &pagefault_prologue );
}

void InterruptManager::InstallIdt()
{
	idtr.Limit = (NUM_IDT_ENTRIES * sizeof(IdtEntry)) - 1;
	idtr.Base = reinterpret_cast<uint64_t>( idtEntries );
	asm volatile( "lidt %0" : : "m"(idtr) );
}

void InterruptManager::DisableInterrupts()
{
	asm volatile( "cli" );
}

void InterruptManager::EnableInterrupts()
{
	asm volatile( "sti" );
}

void InterruptManager::Init()
{
	printf( "InterruptManager::Init()\n" );
	idtEntries = reinterpret_cast<IdtEntry*>( mm.AllocatePage() );

	PopulateIdtEntries();
	InstallIdt();

	EnableInterrupts();
}

