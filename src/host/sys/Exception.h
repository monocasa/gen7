#ifndef GEN7_HOST_SYS_EXCEPTION_H
#define GEN7_HOST_SYS_EXCEPTION_H

#include <execinfo.h>

#include <cstdarg>
#include <cstdio>
#include <stdexcept>

namespace sys {

class Exception : public std::exception
{
private:
	static const int MAX_STRING_SIZE = 1024;
	static const int MAX_FRAMES = 64;

	void* frameArray[ MAX_FRAMES ];

	const int numFrames;

	char** frameNames;

	char * str;

public:
	virtual const char * what() const throw()
	{
		return str;
	}

	int GetNumSymbols() const {
		// Remove Exception's constructor from the list
		return numFrames - 1;
	}

	const char * GetSymbol( int frameNum ) {
		return frameNames[ frameNum + 1 ];
	}

	Exception( const char * format, ... )
	  : numFrames( backtrace( frameArray, MAX_FRAMES ) )
	  , frameNames( backtrace_symbols( frameArray, numFrames ) )
	  , str( new char[MAX_STRING_SIZE] )
	{
		va_list args;
		va_start( args, format );
		vsnprintf( str, MAX_STRING_SIZE, format, args );
		va_end( args );
	}
};

} //namespace sys

#endif //GEN7_HOST_SYS_EXCEPTION_H

