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

	template<typename T>
	T ReadGpr32( int reg ) {
		static_assert( sizeof(T) == sizeof(uint32_t), "Gpr32 needs to be a multiple of four bytes" );
		return ((T*)gprPtr)[ reg ];
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
			else if( (op.args[0].type == CommonOp::Arg::GPR_32) && 
			         (op.args[1].type == CommonOp::Arg::GPR_32) ) {
				SetGpr32<uint32_t>( op.args[0].reg, 
				                    ReadGpr32<uint32_t>(op.args[1].reg) );
				return true;
			}
			sprintf( errorString, "Unknown LOAD_REG combination:  %d <- %d", 
			         op.args[0].type,
			         op.args[1].type );
			return false;
		}

		case CommonOp::OR: {
			if( (op.args[0].type == CommonOp::Arg::GPR_32) && 
			    (op.args[1].type == CommonOp::Arg::GPR_32) &&
			    (op.args[2].type == CommonOp::Arg::IMM_U32) ) {
				const uint32_t result = ReadGpr32<uint32_t>( op.args[1].reg ) | op.args[2].u32;
				SetGpr32<uint32_t>( op.args[0].reg, result );
				return true;
			}
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

