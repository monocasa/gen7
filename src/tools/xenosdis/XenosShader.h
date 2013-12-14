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
		UNKNOWN_SHADER = 0,
		VERTEX_SHADER = 1,
		PIXEL_SHADER = 2,
	};

private:
	enum class Magic : uint32_t {
		PIXEL  = 0x102a1100,
		VERTEX = 0x102a1101,
	};

	struct Header {
		Magic    magic;
		uint32_t offset;
		uint32_t field8;
		uint32_t fieldc;
		uint32_t field10;
		uint32_t constantsOffset;
		uint32_t ucHeaderOffset;
	};

	struct ShaderUcHeader {
		uint32_t offset;
		uint32_t size;
		uint32_t programControl;
		uint32_t contextMisc;
		uint32_t field10;
		uint32_t field14;
		uint32_t field18;
	};

	void Populate();

	const std::vector<uint32_t> binary;

	const Header * const header;
	const ShaderUcHeader * const ucHeader;

	Type type;

public:
	void PrintHeader( FILE *file );
	void PrintUcHeader( FILE *file );

	XenosShader( const uint32_t * const binary, int binarySize )
	  : binary( binary, binary + binarySize )
	  , header( reinterpret_cast<const Header*>( &binary[0] ) )
	  , ucHeader( reinterpret_cast<const ShaderUcHeader*>( 
	                    &binary[ header->ucHeaderOffset / sizeof(uint32_t) ] ) )
	  , type( UNKNOWN_SHADER )
	{
		Populate();
	}
};

} //namespace xenosdis

#endif //GEN7_XENOSDIS_XENOSSHADER_H

