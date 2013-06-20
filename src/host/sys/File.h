#ifndef GEN7_HOST_SYS_FILE_H
#define GEN7_HOST_SYS_FILE_H

#include "sys/Exception.h"

#include <cstdio>

namespace Sys {

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
			uint16_t swap = value << 8;
			swap |= (value >> 8) & 0xFF;
			value = swap;
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
			uint32_t swap = value << 24;
			swap |= (value << 8) & 0xFF0000;
			swap |= (value >> 8) & 0xFF00;
			swap |= (value >> 24) & 0xFF;
			value = swap;
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
			uint64_t swap = value << 56;
			swap |= (value << 40) & 0x00FF000000000000UL;
			swap |= (value << 24) & 0x0000FF0000000000UL;
			swap |= (value <<  8) & 0x000000FF00000000UL;
			swap |= (value >>  8) & 0x00000000FF000000UL;
			swap |= (value >> 24) & 0x0000000000FF0000UL;
			swap |= (value >> 40) & 0x000000000000FF00UL;
			swap |= (value >> 56) & 0x00000000000000FFUL;
			value = swap;
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

} //namespace Sys

#endif //GEN7_HOST_SYS_FILE_H

