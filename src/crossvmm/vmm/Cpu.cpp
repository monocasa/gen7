#include "Cpu.h"
#include "InterInstr.h"

#include <cstdio>

bool Cpu::InterpretIntermediate( InterInstr &instr )
{
	switch( instr.op ) {
	//Misc
		case UNKNOWN_OPCODE: {
			printf( "Unknown opcode %ld (%08lx) @ %016lx\n", instr.args[0], instr.args[1], instr.args[2] );
			return false;
		}

		case NOP: {
			return true;
		}

		case SET_SYS_IMM: {
			SetSystemReg( instr.args[1], instr.args[0] );
		}

		case SET_SYS_REG: {
			uint64_t value = ReadGPR( instr.args[0] );
			return SetSystemReg( instr.args[1], value );
		}

		case READ_SYS: {
			uint64_t value = 0;
			if( !ReadSystemReg( instr.args[1], value ) ) {
				return false;
			}
			SetGPR( instr.args[0], value );
			return true;
		}

		case MOVE_REG: {
			uint64_t value = ReadGPR( instr.args[0] );
			SetGPR( instr.args[1], value );
			return true;
		}

	//Branch
		case BRANCH_ALWAYS: {
			SetPC( instr.args[0] );
			return true;
		}

	//Load/Store
		case LOAD_IMM: {
			SetGPR( instr.args[0], instr.args[1] );
			return true;
		}

	//Logic
		case ANDC: {
			const int sourceReg0 = instr.args[0];
			const int sourceReg1 = instr.args[1];
			const int destReg = instr.args[2];

			uint64_t sourceValue0 = ReadGPR( sourceReg0 );
			uint64_t sourceValue1 = ReadGPR( sourceReg1 );

			uint64_t value = sourceValue0 & ~sourceValue1;

			SetGPR( destReg, value );

			return true;
		}

		case OR: {
			const int sourceReg0 = instr.args[0];
			const int sourceReg1 = instr.args[1];
			const int destReg = instr.args[2];

			uint64_t sourceValue0 = ReadGPR( sourceReg0 );
			uint64_t sourceValue1 = ReadGPR( sourceReg1 );

			uint64_t value = sourceValue0 | sourceValue1;

			SetGPR( destReg, value );

			return true;
		}

		case OR_IMM: {
			uint64_t value = ReadGPR(instr.args[0]);
			value |= instr.args[2];
			SetGPR( instr.args[1], value );
			return true;
		}

	//Shift/Rotate
		case SLL64: {
			uint64_t value = ReadGPR(instr.args[0]);
			value <<= instr.args[2];
			SetGPR( instr.args[1], value );
			return true;
		}

		default: {
			if( instr.op >= PROC_LOW ) {
				return InterpretProcessorSpecific( instr );
			}
			printf( "Unknown instr.op %d (0x%lx, 0x%lx, 0x%lx, 0x%lx)\n",
			        instr.op, instr.args[0], instr.args[1], instr.args[2], instr.args[3] );
			return false;
		}
	}
}

