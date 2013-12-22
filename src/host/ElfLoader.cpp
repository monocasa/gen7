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
		type      = file.Read<uint16_t>();
		machine   = file.Read<uint16_t>();
		version   = file.Read<uint32_t>();
		entry     = file.Read<uint32_t>();
		phoff     = file.Read<uint32_t>();
		shoff     = file.Read<uint32_t>();
		flags     = file.Read<uint32_t>();
		ehsize    = file.Read<uint16_t>();
		phentsize = file.Read<uint16_t>();
		phnum     = file.Read<uint16_t>();
		shentsize = file.Read<uint16_t>();
		shnum     = file.Read<uint16_t>();
		shstrndx  = file.Read<uint16_t>();
	}
	else {
		type      = file.Read<uint16_t>();
		machine   = file.Read<uint16_t>();
		version   = file.Read<uint32_t>();
		entry     = file.Read<uint64_t>();
		phoff     = file.Read<uint64_t>();
		shoff     = file.Read<uint64_t>();
		flags     = file.Read<uint32_t>();
		ehsize    = file.Read<uint16_t>();
		phentsize = file.Read<uint16_t>();
		phnum     = file.Read<uint16_t>();
		shentsize = file.Read<uint16_t>();
		shnum     = file.Read<uint16_t>();
		shstrndx  = file.Read<uint16_t>();
	}
}

void ElfLoader::PHdr::Load( sys::File& file, bool is32Bit )
{
	if( is32Bit ) {
		type   = file.Read<uint32_t>();
		offset = file.Read<uint32_t>();
		vaddr  = file.Read<uint32_t>();
		paddr  = file.Read<uint32_t>();
		filesz = file.Read<uint32_t>();
		memsz  = file.Read<uint32_t>();
		flags  = file.Read<uint32_t>();
		align  = file.Read<uint32_t>();
	}
	else {
		type   = file.Read<uint32_t>();
		flags  = file.Read<uint32_t>();
		offset = file.Read<uint64_t>();
		vaddr  = file.Read<uint64_t>();
		paddr  = file.Read<uint64_t>();
		filesz = file.Read<uint64_t>();
		memsz  = file.Read<uint64_t>();
		align  = file.Read<uint64_t>();
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
				physMem.WritePhys32( phdr.paddr + i, file.Read<uint32_t>() );
			}
		}
	}
}

} //namespace gen7

