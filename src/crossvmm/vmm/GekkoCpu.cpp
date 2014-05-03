#include "GekkoCpu.h"
#include "MemoryManager.h"

#include "jitpp/powerpc/GekkoTranslator.h"
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

void GekkoCpu::DumpState()
{
	printf( " r0: %8x |  r1: %8x |  r2: %8x |  r3: %8x\n", context.gpr[ 0], context.gpr[ 1], context.gpr[ 2], context.gpr[ 3] );
	printf( " r4: %8x |  r5: %8x |  r6: %8x |  r7: %8x\n", context.gpr[ 4], context.gpr[ 5], context.gpr[ 6], context.gpr[ 7] );
	printf( " r8: %8x |  r9: %8x | r10: %8x | r11: %8x\n", context.gpr[ 8], context.gpr[ 9], context.gpr[10], context.gpr[11] );
	printf( "r12: %8x | r13: %8x | r14: %8x | r15: %8x\n", context.gpr[12], context.gpr[13], context.gpr[14], context.gpr[15] );
	printf( "r16: %8x | r17: %8x | r18: %8x | r19: %8x\n", context.gpr[16], context.gpr[17], context.gpr[18], context.gpr[19] );
	printf( "r20: %8x | r21: %8x | r22: %8x | r23: %8x\n", context.gpr[20], context.gpr[21], context.gpr[22], context.gpr[23] );
	printf( "r24: %8x | r25: %8x | r26: %8x | r27: %8x\n", context.gpr[24], context.gpr[25], context.gpr[26], context.gpr[27] );
	printf( "r28: %8x | r29: %8x | r30: %8x | r31: %8x\n", context.gpr[28], context.gpr[29], context.gpr[30], context.gpr[31] );
	printf( " lr: %8x |  pc: %8x\n",  context.lr, context.pc );
}

void GekkoCpu::DumpPosition()
{
	jitpp::PowerPCDisasm disasm;
	char buffer[ 64 ];

	uint32_t * const pos = (uint32_t*)( (uint64_t)context.pc & (~15) ) - 4;

	for( uint32_t *addr = pos; addr < pos + 16; addr++ ) {
		disasm.Disassemble( (uint8_t*)addr, (uint64_t)addr, buffer );
		uint32_t opcode = *addr;
		printf( "%s %08lx : %08x : %s\n", 
		        (uint64_t)addr == context.pc ? "---->" : "     ",
		        (uint64_t)addr, opcode, buffer );
	}
}

void GekkoCpu::Init()
{
	mmu.Init();
}

void GekkoCpu::Execute()
{
	bool executing = true;
	jitpp::CommonOp ops[10];
	int numOps;

	while( executing ) {
		{ //fetch phase
			jitpp::GekkoTranslator translator;

			uint32_t opcode = *((uint32_t*)(uint64_t)context.pc);

			numOps = translator.BuildOps( ops, opcode, context.pc );
		}

		{ //executionPhase
			context.pc += sizeof(uint32_t);

			if( numOps == 0 ) {
				printf( "==== F A I L U R E ====\n" );
				printf( "Op creation failed\n" );
				context.pc -= 4;
				printf( "~~~~ State ~~~~\n" );
				DumpState();
				printf( "~~~~ Position ~~~~\n" );
				DumpPosition();
				executing = false;
			}
			else {
				for( int i = 0; i < numOps; i++ ) {
					if( !ExecuteOp(ops[i]) ) {
						printf( "==== F A I L U R E ====\n" );
						printf( "Interpretation failed:  %s\n", GetErrorString() );
						context.pc -= 4;
						printf( "~~~~ State ~~~~\n" );
						DumpState();
						printf( "~~~~ Position ~~~~\n" );
						DumpPosition();
						executing = false;
						break;
					}
				}
			}
		}
	}
}

