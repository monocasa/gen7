#ifndef GEN7_LIBJITPP_COMMONOPINTERPRETER_H
#define GEN7_LIBJITPP_COMMONOPINTERPRETER_H

#include "jitpp/CommonOp.h"

#include <cstdint>
#include <cstdio>

namespace jitpp {

template<class MemoryPolicy>
class CommonOpInterpreter
{
protected:
	//using

private:
	char errorString[ 256 ];

	void *gprPtr;

	template<typename T>
	void SetGpr32( int reg, T value ) {
		((uint32_t*)gprPtr)[ reg ] = value;
	}

public:
	virtual void SetPc( uint64_t newPc ) = 0;

	bool ExecuteOp( const jitpp::CommonOp &op );

	const char* GetErrorString() const {
		return errorString;
	}

	CommonOpInterpreter( void *gprPtr )
	  : gprPtr( gprPtr )
	{ }
};

template<class MemoryPolicy>
bool CommonOpInterpreter<MemoryPolicy>::ExecuteOp( const jitpp::CommonOp &op )
{
	switch( op.type ) {
		case CommonOp::BRANCH: {
			SetPc( op.args[0].addr );
			return true;
		}

		case CommonOp::LOAD_REG: {
			if( (op.args[0].type == CommonOp::Arg::GPR_32) && 
			    (op.args[1].type == CommonOp::Arg::IMM_U32) ) {
				SetGpr32<uint32_t>( op.args[0].reg, op.args[1].u32 );
				return true;
			}
			sprintf( errorString, "Unknown LOAD_REG combination:  %d <- %d", 
			         op.args[0].type,
			         op.args[1].type );
			return false;
		}

		default: {
			sprintf( errorString, "Unknown CommonOp::Type:  %d", op.type );
			return false;
		}
	}
}

} //namespace jitpp

#endif //GEN7_LIBJITPP_COMMONOP_INTERPRETER_H

