#include "XenosShader.h"

namespace xenosdis {

void XenosShader::Populate()
{
}

void XenosShader::PrintHeader( FILE *file )
{
	fprintf( file, "Magic:  %08x\n", header->magic );
}

} //namespace xenosdis

