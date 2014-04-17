#include "DcRealMemory.h"
#include "GekkoCpu.h"
#include "MemoryManager.h"
#include "InterruptManager.h"
#include "PpcCpu.h"
#include "Sh4aCpu.h"
#include "WiiRealMemory.h"
#include "XenonRealMemory.h"

#include "jitpp/powerpc/XenonCpuContext.h"
#include "jitpp/sh4a/Sh4aCpuContext.h"

#include <cstdint>
#include <cstdio>

typedef uint32_t hypercall_num_t;

extern "C" int hypercall_0( hypercall_num_t callnum );
extern "C" int hypercall_1( hypercall_num_t callnum, uint64_t arg );

uint8_t value;

extern "C"
void hyper_quit()
{
	hypercall_0( 0 );
}

extern "C"
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
	intm.Init();

	jitpp::CpuContext *cpuContext = reinterpret_cast<jitpp::CpuContext*>( 0xFFFFFFFF80003000UL );
	switch( cpuContext->type )
	{
		case jitpp::CpuType::XENON: {
			xenonReal.Init();
			PpcCpu cpu( *static_cast<jitpp::XenonCpuContext*>( cpuContext ) );
			cpu.Init();
			cpu.Execute();
			break;
		}

		case jitpp::CpuType::SH4A: {
			dcReal.Init();
			Sh4aCpu cpu( *static_cast<jitpp::Sh4aCpuContext*>( cpuContext ) );
			cpu.Init();
			cpu.Execute();
			break;
		}

		case jitpp::CpuType::GEKKO: {
			wiiReal.Init();
			GekkoCpu cpu( *static_cast<jitpp::GekkoCpuContext*>( cpuContext ) );
			cpu.Init();
			cpu.Execute();
			break;
		}

		default: {
			printf( "ERROR:  Unknown cpuType:  %d\n", cpuContext->type );
			hyper_quit();
		}
	}

	hyper_quit();
}

