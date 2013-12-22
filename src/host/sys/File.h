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

	uint16_t Read16()
	{
		uint16_t value;

		if( 1 != fread( &value, sizeof(uint16_t), 1, file ) ) {
			throw Exception( "Attempting to read beyond end of file:  %s", path );
		}

		if( endian != SYS_ENDIAN ) {
			value = util::ByteSwap<uint16_t>( value );
		}

		return value;
	}

	uint32_t Read32()
	{
		uint32_t value;

		if( 1 != fread( &value, sizeof(uint32_t), 1, file ) ) {
			throw Exception( "Attempting to read beyond end of file:  %s", path );
		}

		if( endian != SYS_ENDIAN ) {
			value = util::ByteSwap<uint32_t>( value );
		}

		return value;
	}

	uint64_t Read64()
	{
		uint64_t value;

		if( 1 != fread( &value, sizeof(uint64_t), 1, file ) ) {
			throw Exception( "Attempting to read beyond of file:  %s", path );
		}

		if( endian != SYS_ENDIAN ) {
			value = util::ByteSwap<uint64_t>( value );
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

