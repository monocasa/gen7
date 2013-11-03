#ifndef GEN7_HOST_ELFLOADER_H
#define GEN7_HOST_ELFLOADER_H

#include "sys/File.h"

#include <vector>

namespace gen7 {

class PhysicalMemory;

class ElfLoader
{
public:
	enum Target {
		X86_64 = 0,
		POWERPC32 = 1,
		POWERPC64 = 2,
		SH4 = 3,
	};

private:
	struct Hdr {
		static const int EI_MAG0    =  0;
		static const int EI_MAG1    =  1;
		static const int EI_MAG2    =  2;
		static const int EI_MAG3    =  3;
		static const int EI_CLASS   =  4;
		static const int EI_DATA    =  5;
		static const int EI_VERSION =  6;
		static const int EI_NIDENT  = 16;

		static const int ELFCLASSNONE = 0;
		static const int ELFCLASS32   = 1;
		static const int ELFCLASS64   = 2;

		static const int ELFDATANONE = 0;
		static const int ELFDATA2LSB = 1;
		static const int ELFDATA2MSB = 2;

		unsigned char ident[EI_NIDENT];

		uint16_t type;
		uint16_t machine;
		uint32_t version;
		uint64_t entry;
		uint64_t phoff;
		uint64_t shoff;
		uint32_t flags;
		uint16_t ehsize;
		uint16_t phentsize;
		uint16_t phnum;
		uint16_t shentsize;
		uint16_t shnum;
		uint16_t shstrndx;

		bool IsValid() {
			return (ident[EI_MAG0] == 0x7F) && (ident[EI_MAG1] == 'E') &&
			        (ident[EI_MAG2] == 'L') && (ident[EI_MAG3] == 'F');
		}

		bool Is32Bit() {
			return ident[EI_CLASS] == ELFCLASS32;
		}

		bool IsLittleEndian() {
			return ident[EI_DATA] == ELFDATA2LSB;
		}

		void Load( Sys::File& file );
	};

	struct PHdr {
		static const uint32_t PT_LOAD = 1;

		uint32_t type;
		uint64_t offset;
		uint64_t vaddr;
		uint64_t paddr;
		uint64_t filesz;
		uint64_t memsz;
		uint32_t flags;
		uint64_t align;

		void Load( Sys::File& file, bool is32Bit );
	};

	static const char * TAG;

	const char * exePath;
	Sys::File file;
	Target target;

	Hdr header;

	std::vector<PHdr> programHeaders;

public:
	void Load( PhysicalMemory& physMem );

	uint64_t GetEntry() {
		return header.entry;
	}

	ElfLoader( const char * exePath, Target target )
	  : exePath( exePath )
	  , file( exePath )
	  , target( target )
	{ }
};

} //namespace gen7

#endif //GEN7_HOST_ELFLOADER_H

