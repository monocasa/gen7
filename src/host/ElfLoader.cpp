#include "sys/Exception.h"
#include "sys/File.h"
#include "ElfLoader.h"
#include "Log.h"
#include "PhysicalMemory.h"

namespace gen7 {

const char * ElfLoader::TAG = "ELFLDR";

void ElfLoader::Hdr::Load( sys::File& file )
{
	file.ReadBinary( ident, sizeof(ident) );

	file.SetEndian( IsLittleEndian() ? sys::File::LITTLE : sys::File::BIG );

	if( Is32Bit() ) {
		type      = file.Read16();
		machine   = file.Read16();
		version   = file.Read32();
		entry     = file.Read32();
		phoff     = file.Read32();
		shoff     = file.Read32();
		flags     = file.Read32();
		ehsize    = file.Read16();
		phentsize = file.Read16();
		phnum     = file.Read16();
		shentsize = file.Read16();
		shnum     = file.Read16();
		shstrndx  = file.Read16();
	}
	else {
		type      = file.Read16();
		machine   = file.Read16();
		version   = file.Read32();
		entry     = file.Read64();
		phoff     = file.Read64();
		shoff     = file.Read64();
		flags     = file.Read32();
		ehsize    = file.Read16();
		phentsize = file.Read16();
		phnum     = file.Read16();
		shentsize = file.Read16();
		shnum     = file.Read16();
		shstrndx  = file.Read16();
	}
}

void ElfLoader::PHdr::Load( sys::File& file, bool is32Bit )
{
	if( is32Bit ) {
		type   = file.Read32();
		offset = file.Read32();
		vaddr  = file.Read32();
		paddr  = file.Read32();
		filesz = file.Read32();
		memsz  = file.Read32();
		flags  = file.Read32();
		align  = file.Read32();
	}
	else {
		type   = file.Read32();
		flags  = file.Read32();
		offset = file.Read64();
		vaddr  = file.Read64();
		paddr  = file.Read64();
		filesz = file.Read64();
		memsz  = file.Read64();
		align  = file.Read64();
	}
}

void ElfLoader::Load( PhysicalMemory& physMem )
{
	DPRINT( TAG, "Loading %s\n", exePath );

	if( !file.IsOpen() ) {
		throw sys::Exception( "Unable to open file %s", exePath );
	}

	header.Load( file );

	for( int i = 0; i < header.phnum; i++ ) {
		file.Seek( header.phoff + (i * header.phentsize ) );

		PHdr phdr;
		phdr.Load( file, header.Is32Bit() );

		programHeaders.push_back( phdr );
	}

	for( const auto &phdr : programHeaders ) {
		if( PHdr::PT_LOAD == phdr.type ) {
			DPRINT( TAG, " : %lx bytes %08lx -> %08lx\n", phdr.filesz, phdr.offset, phdr.paddr );
			file.Seek( phdr.offset );

			for( uint64_t i = 0; i < phdr.filesz; i += sizeof(uint32_t) ) {
				physMem.WritePhys32( phdr.paddr + i, file.Read32() );
			}
		}
	}
}

} //namespace gen7

