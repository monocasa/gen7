#include "XenosShader.h"

#include <stdexcept>

namespace xenosdis {

void XenosShader::Populate()
{
	switch( header->magic ) {
		case Magic::PIXEL: {
			type = PIXEL_SHADER;
			break;
		}

		case Magic::VERTEX: {
			type = VERTEX_SHADER;
			break;
		}

		default: {
			throw std::runtime_error( "Unknown magic" );
		}
	}
}

void XenosShader::PrintHeader( FILE *file )
{
	fprintf( file, "           Magic:  %08x\n", header->magic );
	fprintf( file, "          Offset:  %08x\n", header->offset );
	fprintf( file, "  UNKN Field 0x8:  %08x\n", header->field8 );
	fprintf( file, "  UNKN Field 0xC:  %08x\n", header->fieldc );
	fprintf( file, " UNKN Field 0x10:  %08x\n", header->field10 );
	fprintf( file, "Constants Offset:  %08x\n", header->constantsOffset );
	fprintf( file, "uC Header Offset:  %08x\n", header->ucHeaderOffset );
}

void XenosShader::PrintUcHeader( FILE *file )
{
	fprintf( file, "         Offset:  %08x\n", ucHeader->offset );
	fprintf( file, "           Size:  %08x\n", ucHeader->size );
	fprintf( file, "Program Control:  %08x\n", ucHeader->programControl );
	fprintf( file, "   Context Misc:  %08x\n", ucHeader->contextMisc );
	fprintf( file, "UNKN Field 0x10:  %08x\n", ucHeader->field10 );
	fprintf( file, "UNKN Field 0x14:  %08x\n", ucHeader->field14 );
	fprintf( file, "UNKN Field 0x18:  %08x\n", ucHeader->field18 );
}

} //namespace xenosdis

