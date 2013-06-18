#include "sys/Exception.h"
#include "sys/Mem.h"

#include <cstdlib>

namespace Sys {

void * AllocatePageMem( size_t size )
{
	void *memptr;

	int status = posix_memalign( &memptr, 4096, size );
	if( EINVAL == status ) {
		throw Exception( "Page size of 4096 is not a valid alignment for posix_memalign" );
	}
	else if( ENOMEM == status ) {
		throw Exception( "No memory for posix_memalign" );
	}
	else if( 0 == status ) {
		return memptr;
	}
	else {
		throw Exception( "Unknown status %d thrown by posix_memalign", status );
	}
}

} //namespace Sys

