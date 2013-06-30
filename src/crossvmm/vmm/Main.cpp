#include "MemoryManager.h"
#include "XenonRealMemory.h"

#include "shared/Types.h"

#include <cstdint>
#include <cstdio>

typedef uint32_t hypercall_num_t;

extern "C" int hypercall_0( hypercall_num_t callnum );
extern "C" int hypercall_1( hypercall_num_t callnum, uint64_t arg );

uint8_t value;

void hyper_quit()
{
	hypercall_0( 0 );
}

int hyper_putc( char data )
{
	return hypercall_1( 1, data );
}

void puts( const char * str )
{
	while( *str != '\0' ) {
		hyper_putc( *str );
		str++;
	}
}

extern "C" void* GetCR0();
extern "C" void* GetCR2();
extern "C" void* GetCR3();
extern "C" void* GetCR4();

extern "C" void kmain()
{
	printf( "~~~~~~~~ Initializing VMM ~~~~~~~~\n" );
	mm.Init();
	xenonReal.Init();
	Gen7::XenonPpcContext *ppcContext = reinterpret_cast<Gen7::XenonPpcContext*>( 0xFFFFFFFF80003000UL );
	printf( "PowerPC pc = 0x%016lx\n", ppcContext->pc );
	printf( "~~~~~~~~ VMM Initialization Complete ~~~~~~~~\n" );

	hyper_quit();
}

