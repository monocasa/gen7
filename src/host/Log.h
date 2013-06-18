#ifndef GEN7_HOST_LOG_H
#define GEN7_HOST_LOG_H

#include <cstdarg>
#include <cstdio>

namespace Gen7 {

static inline void DPRINT( const char * tag, const char * format, ... )
{
	printf( "%s:  ", tag );

	va_list args;
	va_start( args, format );
	vprintf( format, args );
	va_end( args );
}

} //namespace Gen7

#endif //GEN7_HOST_LOG_H

