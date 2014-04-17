#include "GekkoCpu.h"
#include "MemoryManager.h"

#include "jitpp/powerpc/PowerPCDisasm.h"

#include <cstdio>

void GekkoCpu::PowerPC32Mmu::Init()
{
	pml3 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	cachedPml2 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );
	uncachedPml2 = reinterpret_cast<uint64_t*>( mm.AllocatePage() );

	for( int i = 0; i < 128; i++ ) {
		uint64_t offset = i * (2 * 1024 * 1024UL);
		uint64_t physaddr = 0x100000000UL | offset;
		cachedPml2[ i ] = physaddr | 0x83;
		uncachedPml2[ i ] = physaddr | 0x83;
	}

	for( int i = 128; i < 512; i++ ) {
		cachedPml2[ i ] = 0;
		uncachedPml2[ i ] = 0;
	}

	pml3[2] = (((uint64_t)cachedPml2) - 0xFFFFFFFF80000000UL) | 1;
	pml3[3] = (((uint64_t)uncachedPml2) - 0xFFFFFFFF80000000UL) | 1;

	mm.SetLowerPml3( pml3, 0x0000000000000000UL );
}

void GekkoCpu::Init()
{
	mmu.Init();
}

void GekkoCpu::Execute()
{
	printf( "PC:  %08x\n", context.pc );
	printf( "OP:  %08x\n", *((uint32_t*)((uint64_t)context.pc)) );

	for( uint64_t i = context.pc; i < (context.pc + 0x80); i += sizeof(uint32_t) ) {
		jitpp::PowerPCDisasm disasm;
		char strBuffer[64];
		uint32_t opcode;

		opcode = *((uint32_t*)i);

		disasm.Disassemble( (uint8_t*)&opcode, i, strBuffer );
		printf( "%08lx:  %08x - %s\n", i, opcode, strBuffer );
	}
}

