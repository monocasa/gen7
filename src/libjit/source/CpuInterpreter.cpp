#include "jit/CpuInterpreter.h"
#include "jit/InterInstr.h"

#include <cstdio>

namespace jit {

bool CpuInterpreter::InterpretIntermediate( InterInstr &instr )
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

		case MOVE_REG: {
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

		case BRANCH_GPR_NOT_ZERO: {
			const int gpr = instr.args[0];
			const uint64_t target = instr.args[1];

			uint64_t value = ReadGPR64(gpr);

			if( value != 0 ) {
				SetPC( target );
			}

			return true;
		}

	//Load/Store
		case LOAD_IMM: {
			const int gpr = instr.args[0];
			const uint64_t imm = instr.args[1];

			SetGPR64( gpr, imm );
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

