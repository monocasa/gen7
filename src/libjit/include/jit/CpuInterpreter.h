#ifndef GEN7_LIBJIT_CPUINTERPRETER_H
#define GEN7_LIBJIT_CPUINTERPRETER_H

#include "jit/InterInstr.h"

#include "util/BitOps.h"

#include <cstdint>
#include <cstdio>

namespace jit {

template<class MemoryPolicy>
class CpuInterpreter : public MemoryPolicy
{
protected:
	using MemoryPolicy::ReadMem32;
	using MemoryPolicy::ReadMem64;
	using MemoryPolicy::WriteMem8;
	using MemoryPolicy::WriteMem32;
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
	bool ReadReg( const Operand<int> &source, uint64_t &value ) {
		int reg = *source;

		switch( source.type ) {
			case OpType::SYS64: {
				return ReadSystemReg( reg, value );
			}

			case OpType::GPR32: {
				value = ReadGPR32( reg );
				return true;
			}

			case OpType::GPR64: {
				value = ReadGPR64( reg );
				return true;
			}

			default: {
				sprintf( errorString, "Unknown opType %d in ReadReg( %d:%d, value )\n",
				         source.type,
				         source.type,
				         *source );
				return false;
			}
		}
	}

	bool SetReg( const Operand<int> &dest, uint64_t value ) {
		int reg = *dest;

		switch( dest.type ) {
			case OpType::SYS64: {
				return SetSystemReg( reg, value );
			}

			case OpType::GPR32: {
				SetGPR32( reg, value );
				return true;
			}

			case OpType::GPR64: {
				SetGPR64( reg, value );
				return true;
			}

			default: {
				sprintf( errorString, "Unknown opType %d in SetSource( %d:%d, 0x%lx )\n",
				         dest.type,
				         dest.type,
				         *dest,
				         value );
				return false;
			}
		}
	}

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
			sprintf( errorString, "Unknown opcode %d (%08lx) @ %016lx", 
			         *instr.unknown.opcodeCookie,
			         *instr.unknown.instruction,
			         *instr.unknown.pc );
			return false;
		}

		case NOP: {
			return true;
		}

		case MOVE_REG: {
			uint64_t value;

			if( !ReadReg(instr.twoReg.source, value) ) {
				return false;
			}

			return SetReg( instr.twoReg.dest, value );
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

		case BRANCH_GPR64_NOT_ZERO_GPR64: {
			const int gpr = instr.args[0];
			const int targetReg = instr.args[1];

			uint64_t value = ReadGPR64( gpr );

			if( value != 0 ) {
				SetPC( ReadGPR64(targetReg) );
			}

			return true;
		}

	//Load/Store
		case LD_32_IMM: {
			return SetReg( instr.imm32.dest, *instr.imm32.source );
		}

		case LD_64_IMM: {
			return SetReg( instr.imm64.dest, *instr.imm64.source );
		}

		case LD_32_ABS: {
			const uint32_t value = ReadMem32( *instr.imm64.source );

			return SetReg( instr.imm64.dest, value );
		}

		case LD_32_REG_OFF: {
			uint64_t base;

			if( !ReadReg( instr.ldStIdx.addrReg, base ) ) {
				return false;
			}

			const uint32_t value = ReadMem32( base + *instr.ldStIdx.offset );

			return SetReg( instr.ldStIdx.valueReg, value );
		}

		case LD_64_REG_OFF: {
			int destReg = instr.args[0];
			int addrReg = instr.args[1];
			int64_t offset = instr.args[2];

			const uint64_t value = ReadMem64( offset + ReadGPR64(addrReg) );

			SetGPR64( destReg, value );

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

		case ST_8_REG: {
			int sourceReg = instr.args[0];
			int addrReg = instr.args[1];

			WriteMem8( ReadGPR64(addrReg), (uint8_t)ReadGPR32(sourceReg) );

			return true;
		}

		case ST_32_REG: {
			int sourceReg = instr.args[0];
			int addrReg = instr.args[1];

			WriteMem32( ReadGPR64(addrReg), ReadGPR32(sourceReg) );

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
		case AND_IMM_32: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const uint32_t imm = instr.args[2];

			uint32_t sourceValue = ReadGPR32( sourceReg );

			uint32_t result = sourceValue & imm;

			SetGPR32( destReg, result );

			return true;
		}

		case AND_IMM_64: {
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

		case ANDC_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const uint64_t imm = instr.args[2];

			uint64_t sourceValue = ReadGPR64( sourceReg );

			uint64_t result = sourceValue & ~imm;

			SetGPR64( destReg, result );

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
		}

		case XOR_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const uint64_t imm = instr.args[2];

			uint64_t sourceValue = ReadGPR64( sourceReg );

			uint64_t result = sourceValue ^ imm;

			SetGPR64( destReg, result );

			return true;
		}

	//Shift/Rotate
		case ROL32_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const int rotate = instr.args[2];

			const uint32_t sourceValue = ReadGPR32( sourceReg );

			uint32_t result = (sourceValue << rotate) |
			                  (sourceValue >> (32 - rotate));

			SetGPR32( destReg, result );

			return true;
		}

		case ROL64_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const int rotate = instr.args[2];

			const uint64_t sourceValue = ReadGPR64( sourceReg );

			uint64_t result = (sourceValue << rotate) |
			                  (sourceValue >> (64 - rotate));

			SetGPR64( destReg, result );

			return true;
		}
		
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

		case SLR32_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const int shift = instr.args[2];

			uint32_t result = ReadGPR32( sourceReg ) >> shift;

			SetGPR32( destReg, result );

			return true;
		}
			
		case SLR64_IMM: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];
			const int shift = instr.args[2];

			uint64_t result = ReadGPR64( sourceReg ) >> shift;

			SetGPR64( destReg, result );
			return true;
		}

		case BYTE_SWAP_32: {
			const int sourceReg = instr.args[0];
			const int destReg = instr.args[1];

			uint32_t result = util::ByteSwap<uint32_t>( ReadGPR32(sourceReg) );

			SetGPR32( destReg, result );
			return true;
		}

		default: {
			if( instr.op >= PROC_LOW ) {
				return InterpretProcessorSpecific( instr );
			}
			sprintf( errorString, "Unknown instr.op %d\n", instr.op );
			return false;
		}
	}
}

} //namespace jit

#endif //GEN7_LIBJIT_CPUINTERPRETER_H

