#ifndef GEN7_HOST_SYS_FILE_H
#define GEN7_HOST_SYS_FILE_H

#include "sys/Exception.h"

#include "util/BitOps.h"

#include <cstdio>

namespace sys {

class File
{
public:
	enum Endian {
		BIG,
		LITTLE
	};

private:
	static const Endian SYS_ENDIAN = LITTLE;

	const char * path;
	FILE* file;
	Endian endian;

public:
	bool IsOpen()
	{
		return (nullptr != file);
	}

	void SetEndian( Endian endian )
	{
		this->endian = endian;
	}

	void Seek( uint64_t offset ) {
		fseek( file, offset, SEEK_SET );
	}

	void ReadBinary( unsigned char * buffer, const size_t size )
	{
		if( 1 != fread( buffer, size, 1, file ) ) {
			throw Exception( "Attempting to read beyond end of file:  %s", path );
		}
	}

	template<typename T>
	T Read()
	{
		T value;

		if( 1 != fread(&value, sizeof(T), 1, file) ) {
			throw Exception( "Attempting to read beyond end of file:  %s", path );
		}

		if( endian != SYS_ENDIAN ) {
			value = util::ByteSwap<T>( value );
		}

		return value;
	}

	size_t Size() {
		long current = ftell( file );

		fseek( file, 0, SEEK_END );

		long last = ftell( file );

		fseek( file, current, SEEK_SET );

		return last;
	}

	File( const char * path )
	  : path( path )
	  , file( fopen( path, "rb" ) )
	  , endian( LITTLE )
	{ }
};

} //namespace sys

#endif //GEN7_HOST_SYS_FILE_H

