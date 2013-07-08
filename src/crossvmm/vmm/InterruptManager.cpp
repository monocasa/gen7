#include "InterruptManager.h"
#include "MemoryManager.h"

#include <cstdio>

InterruptManager intm;

extern "C" void isr();

void InterruptManager::PopulateIdtEntries()
{
	uint64_t isrPtr = reinterpret_cast<uint64_t>( &isr );

	printf( "isr @ 0x%016lx\n", isrPtr );
	idtEntries[3].offset_15_0  = isrPtr;
	idtEntries[3].offset_31_16 = isrPtr >> 16;
	idtEntries[3].offset_63_32 = isrPtr >> 32;
	idtEntries[3].selector = 0x08;
	idtEntries[3].flags = 0x8E00;
}

void InterruptManager::InstallIdt()
{
	idtr.Limit = 4095;
	idtr.Base = reinterpret_cast<uint64_t>( idtEntries );
	asm volatile( "lidt %0" : : "m"(idtr) );

	printf( "Installed IDT\n" );
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

	asm volatile( "int3" );
}

