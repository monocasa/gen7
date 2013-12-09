#ifndef GEN7_XENOSDIS_XENOSSHADER_H
#define GEN7_SENOSDIS_XENOSSHADER_H

#include <cstdint>
#include <cstdio>
#include <vector>

namespace xenosdis {

class XenosShader
{
public:
	class XenosInstruction {
	};

	enum Type {
		VERTEX_SHADER = 0,
		PIXEL_SHADER = 1,
	};

private:
	struct Header {
		uint32_t magic;
		uint32_t offset;
		uint32_t field_8;
		uint32_t field_c;
		uint32_t field_10;
		uint32_t offsetConstants;
		uint32_t offsetShader;
	};

	void Populate();

	const std::vector<uint32_t> binary;

	const Header * const header;

public:
	void PrintHeader( FILE *file );

	XenosShader( const uint32_t * const binary, int binarySize )
	  : binary( binary, binary + binarySize )
	  , header( reinterpret_cast<const Header*>( &binary[0] ) )
	{
		Populate();
	}
};

} //namespace xenosdis

#endif //GEN7_XENOSDIS_XENOSSHADER_H

