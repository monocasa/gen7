#ifndef GEN7_LIBJIT_CPUINTERPRETER_H
#define GEN7_LIBJIT_CPUINTERPRETER_H

#include "jit/InterInstr.h"

#include <cstdint>
#include <cstdio>

namespace jit {

template<class MemoryPolicy>
class CpuInterpreter : public MemoryPolicy
{
	using MemoryPolicy::ReadMem32;
	using MemoryPolicy::WriteMem64;

protected:
	uint8_t * const GPR_PTR;

	char errorString[ 128 ];

	uint32_t ReadGPR32( int offset ) {
		return *reinterpret_cast<uint32_t*>( &GPR_PTR[offset] );
	}

	uint64_t ReadGPR64( int offset ) {
		return *reinterpret_cast<uint64_t*>( &GPR_PTR[offset] );
	}

	void SetGPR32( int offset, uint32_t value ) {
		*reinterpret_cast<uint32_t*>( &GPR_PTR[offset] ) = value;
	}

	void SetGPR64( int offset, uint64_t value ) {
		*reinterpret_cast<uint64_t*>( &GPR_PTR[offset] ) = value;
	}

	bool &isReserved;
	uint64_t &reservation;

public:
	virtual void SetPC( uint64_t newPc ) = 0;

	virtual bool SetSystemReg( int sysReg, uint64_t value ) = 0;
	virtual bool ReadSystemReg( int sysReg, uint64_t &value ) = 0;

	virtual bool InterpretProcessorSpecific( jit::InterInstr &instr ) = 0;

	bool InterpretIntermediate( jit::InterInstr &instr );

	const char * GetErrorString() const {
		return errorString;
	}

	CpuInterpreter( void* gprPtr, bool &isReserved, uint64_t &reservation )
	  : GPR_PTR( reinterpret_cast<uint8_t*>( gprPtr ) )
	  , isReserved( isReserved )
	  , reservation( reservation )
	{
		sprintf( errorString, "No Error" );
	}
};

template<class MemoryPolicy>
bool CpuInterpreter<MemoryPolicy>::InterpretIntermediate( InterInstr &instr )
{
	switch( instr.op ) {
	//Misc
		case UNKNOWN_OPCODE: {
			sprintf( errorString, "Unknown opcode %ld (%08lx) @ %016lx", 
			         instr.args[0], instr.args[1], instr.args[2] );
			return false;
		}

		case NOP: {
			return true;
		}

		case SET_SYS_IMM: {
			const uint64_t imm = instr.args[0];
			const int sourceReg = instr.args[1];

			return SetSystemReg( sourceReg, imm );
		}

		case SET_SYS_REG: {
			const int sourceReg = instr.args[0];
			const int sysReg = instr.args[1];

			uint64_t result = ReadGPR64( sourceReg );
			return SetSystemReg( sysReg, result );
		}

		case READ_SYS: {
			const int destReg = instr.args[0];
			const int sysReg = instr.args[1];

			uint64_t result = 0;
			if( !ReadSystemReg( sysReg, result ) ) {
				return false;
			}
			SetGPR64( destReg, result );
			return true;
		}

		case MOVE_REG32: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];

			uint32_t result = ReadGPR32( sourceReg );
			SetGPR32( destReg, result );
			return true;
		}

		case MOVE_REG64: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];

			uint64_t result = ReadGPR64( sourceReg );
			SetGPR64( destReg, result );
			return true;
		}

	//Branch
		case BRANCH_ALWAYS: {
			const uint64_t target = instr.args[0];

			SetPC( target );
			return true;
		}

		case BRANCH_GPR64: {
			const int gpr = instr.args[0];

			SetPC( ReadGPR64(gpr) );

			return true;
		}

		case BRANCH_GPR32_MASK_ZERO: {
			const int gpr = instr.args[0];
			const uint32_t mask = instr.args[1];
			const uint64_t target = instr.args[2];

			if( (ReadGPR32(gpr) & mask) == 0 ) {
				SetPC( target );
			}

			return true;
		}

		case BRANCH_GPR32_MASK_NOT_ZERO: {
			const int gpr = instr.args[0];
			const uint32_t mask = instr.args[1];
			const uint64_t target = instr.args[2];

			if( (ReadGPR32(gpr) & mask) != 0 ) {
				SetPC( target );
			}

			return true;
		}

		case BRANCH_GPR64_NOT_ZERO: {
			const int gpr = instr.args[0];
			const uint64_t target = instr.args[1];

			uint64_t value = ReadGPR64(gpr);

			if( value != 0 ) {
				SetPC( target );
			}

			return true;
		}

	//Load/Store
		case LD_32_IMM: {
			const int gpr = instr.args[0];
			const uint32_t imm = instr.args[1];

			SetGPR32( gpr, imm );
			return true;
		}

		case LD_64_IMM: {
			const int gpr = instr.args[0];
			const uint64_t imm = instr.args[1];

			SetGPR64( gpr, imm );
			return true;
		}

		case LD_32_L: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];

			const uint64_t addr = ReadGPR64( sourceReg );

			reservation = addr;
			isReserved = true;

			const uint32_t value = ReadMem32( addr );

			SetGPR32( destReg, value );
			return true;
		}

		case LD_32_IDX_L: {
			const int sourceReg = instr.args[0];
			const int offsetReg = instr.args[1];
			const int destReg = instr.args[2];

			const uint64_t addr = ReadGPR64( sourceReg ) + ReadGPR64( offsetReg );

			reservation = addr;
			isReserved = true;

			const uint32_t value = ReadMem32( addr );

			SetGPR32( destReg, value );
			return true;
		}

		case ST_64: {
			int sourceReg = instr.args[0];
			uint64_t addr = instr.args[1];

			const uint64_t value = ReadGPR64( sourceReg );

			WriteMem64( addr, value );

			return true;
		}

		case ST_64_REG_OFF: {
			int sourceReg = instr.args[0];
			int addrReg = instr.args[1];
			int64_t offset = instr.args[2];

			const uint64_t addr = ReadGPR64( addrReg ) + offset;

			const uint64_t value = ReadGPR64( sourceReg );

			WriteMem64( addr, value );

			return true;
		}

	//Arithmetic
		case ADD: {
			const int sourceReg0 = instr.args[0];
			const int sourceReg1 = instr.args[1];
			const int destReg = instr.args[2];

			uint64_t sourceValue0 = ReadGPR64( sourceReg0 );
			uint64_t sourceValue1 = ReadGPR64( sourceReg1 );

			uint64_t result = sourceValue0 + sourceValue1;

			SetGPR64( destReg, result );

			return true;
		}

		case ADD_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const uint64_t imm = instr.args[2];

			uint64_t sourceValue = ReadGPR64( sourceReg );

			uint64_t result = sourceValue + imm;

			SetGPR64( destReg, result );

			return true;
		}

		case SUB: {
			const int sourceReg0 = instr.args[0];
			const int sourceReg1 = instr.args[1];
			const int destReg = instr.args[2];

			uint64_t sourceValue0 = ReadGPR64( sourceReg0 );
			uint64_t sourceValue1 = ReadGPR64( sourceReg1 );

			uint64_t result = ~sourceValue0 + sourceValue1 + 1;

			SetGPR64( destReg, result );

			return true;
		} 

		case SUBU_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const uint64_t imm = instr.args[2];

			uint64_t sourceValue = ReadGPR64( sourceReg );

			uint64_t result = sourceValue - imm;

			SetGPR64( destReg, result );

			return true;
		}

	//Logic
		case AND_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const uint64_t imm = instr.args[2];

			uint64_t sourceValue = ReadGPR64( sourceReg );

			uint64_t result = sourceValue & imm;

			SetGPR64( destReg, result );

			return true;
		}

		case ANDC: {
			const int sourceReg0 = instr.args[0];
			const int sourceReg1 = instr.args[1];
			const int destReg = instr.args[2];

			uint64_t sourceValue0 = ReadGPR64( sourceReg0 );
			uint64_t sourceValue1 = ReadGPR64( sourceReg1 );

			uint64_t value = sourceValue0 & ~sourceValue1;

			SetGPR64( destReg, value );

			return true;
		}

		case OR: {
			const int sourceReg0 = instr.args[0];
			const int sourceReg1 = instr.args[1];
			const int destReg = instr.args[2];

			uint64_t sourceValue0 = ReadGPR64( sourceReg0 );
			uint64_t sourceValue1 = ReadGPR64( sourceReg1 );

			uint64_t value = sourceValue0 | sourceValue1;

			SetGPR64( destReg, value );

			return true;
		}

		case OR_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const uint64_t imm = instr.args[2];

			uint64_t sourceValue = ReadGPR64( sourceReg );

			uint64_t result = sourceValue | imm;

			SetGPR64( destReg, result );

			return true;
			return true;
		}

	//Shift/Rotate
		case SLL32: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const int shiftReg = instr.args[2];

			uint32_t result = ReadGPR32( sourceReg ) << ReadGPR32( shiftReg );

			SetGPR32( destReg, result );

			return true;
		}

		case SLL32_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const int shift = instr.args[2];

			uint32_t result = ReadGPR32( sourceReg ) << shift;

			SetGPR32( destReg, result );

			return true;
		}
			
		case SLL64_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const int shift = instr.args[2];

			uint64_t result = ReadGPR64( sourceReg ) << shift;

			SetGPR64( destReg, result );
			return true;
		}

		default: {
			if( instr.op >= PROC_LOW ) {
				return InterpretProcessorSpecific( instr );
			}
			sprintf( errorString, "Unknown instr.op %d (0x%lx, 0x%lx, 0x%lx, 0x%lx)\n",
			         instr.op, instr.args[0], instr.args[1], instr.args[2], instr.args[3] );
			return false;
		}
	}
}

} //namespace jit

#endif //GEN7_LIBJIT_CPUINTERPRETER_H

