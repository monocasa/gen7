#ifndef GEN7_HOST_SUBSYSTEM_H
#define GEN7_HOST_SUBSYSTEM_H

#include <cstdarg>
#include <cstdio>

namespace gen7 {

class MachineContext;

class Subsystem
{
protected:
	MachineContext& context;

public:
	enum class InitPhase : int {
		ALLOCATION = 0,
		APP_1 = 1,
		APP_2 = 2,

		FIRST = ALLOCATION,
		LAST = APP_2,
	};

	virtual void Init( InitPhase phase ) = 0;

	virtual const char * GetName() const = 0;
	virtual const char * GetShortName() const = 0;

	virtual void DPRINT( const char * format, ... ) const {
		printf( "%s:  ", GetShortName() );
		va_list args;
		va_start( args, format );
		vprintf( format, args );
		va_end( args );
	}

	Subsystem( MachineContext& context );
};

} //namespace gen7

#endif //GEN7_HOST_SUBSYSTEM_H

