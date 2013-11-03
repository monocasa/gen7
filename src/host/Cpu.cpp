#include "Cpu.h"

namespace gen7 {

std::string IrInstruction::AsString() const
{
	static const int BUFFER_SIZE = 128;
	char buffer[ BUFFER_SIZE ];

	switch( op ) {
		case ADD: {
			snprintf( buffer, BUFFER_SIZE, "ADD              r%ld, r%ld, r%ld", arg0, arg1, arg2 );
			break;
		}

		case ADD_IMM: {
			snprintf( buffer, BUFFER_SIZE, "add_imm          r%ld, r%ld, 0x%04lx", arg0, arg1, arg2 );
			break;
		}

		case AND_IMM: {
			snprintf( buffer, BUFFER_SIZE, "and_imm          r%ld, r%ld, 0x%04lx", arg0, arg1, arg2 );
			break;
		}

		case ANDC: {
			snprintf( buffer, BUFFER_SIZE, "andc             r%ld, r%ld, r%ld", arg0, arg1, arg2 );
			break;
		}

		case BDNZ: {
			snprintf( buffer, BUFFER_SIZE, "bdnz             loc_%08lx", arg0 );
			break;
		}

		case BEQ: {
			snprintf( buffer, BUFFER_SIZE, "beq              loc_%08lx", arg0 );
			break;
		}

		case BLR: {
			snprintf( buffer, BUFFER_SIZE, "blr" );
			break;
		}

		case BNE: {
			snprintf( buffer, BUFFER_SIZE, "bne              loc_%08lx", arg0 );
			break;
		}

		case BRANCH: {
			snprintf( buffer, BUFFER_SIZE, "branch           loc_%08lx", arg0 );
			break;
		}

		case BRANCH_LINK: {
			snprintf( buffer, BUFFER_SIZE, "branch_link      loc_%08lx", arg0 );
			break;
		}

		case CMPLW_IMM: {
			snprintf( buffer, BUFFER_SIZE, "cmplw_imm        r%ld, 0x%lx", arg0, arg1 );
			break;
		}

		case EIEIO: {
			snprintf( buffer, BUFFER_SIZE, "eieio" );
			break;
		}

		case ISYNC: {
			snprintf( buffer, BUFFER_SIZE, "isync" );
			break;
		}

		case LOAD_IMM: {
			snprintf( buffer, BUFFER_SIZE, "load_imm         r%ld, 0x%lx", arg0, arg1 );
			break;
		}

		case LWAR: {
			snprintf( buffer, BUFFER_SIZE, "lwar             r%ld, r%ld", arg0, arg1 );
			break;
		}

		case LWARX: {
			snprintf( buffer, BUFFER_SIZE, "lwarx            r%ld, r%ld, r%ld", arg0, arg1, arg2 );
			break;
		}

		case MFMSR: {
			snprintf( buffer, BUFFER_SIZE, "mfmsr            r%ld", arg0 );
			break;
		}

		case MFSPR: {
			switch( arg1 ) {
				case 318: {
					snprintf( buffer, BUFFER_SIZE, "mfspr            r%ld, lpcr", arg0 );
					break;
				}

				case 1017: {
					snprintf( buffer, BUFFER_SIZE, "mfspr            r%ld, hid6", arg0 );
					break;
				}

				case 1023: {
					snprintf( buffer, BUFFER_SIZE, "mfspr            r%ld, pir", arg0 );
					break;
				}

				default: {
					snprintf( buffer, BUFFER_SIZE, "mfspr            r%ld, %ld", arg0, arg1 );
					break;
				}
			}
			break;
		}

		case MTCTR: {
			snprintf( buffer, BUFFER_SIZE, "mtctr            r%ld", arg0 );
			break;
		}

		case MTSPR: {
			switch( arg1 ) {
				case 26: {
					snprintf( buffer, BUFFER_SIZE, "mtpsr            r%ld, srr0", arg0 );
					break;
				}

				case 27: {
					snprintf( buffer, BUFFER_SIZE, "mtpsr            r%ld, srr1", arg0 );
					break;
				}

				case 273: {
					snprintf( buffer, BUFFER_SIZE, "mtpsr            r%ld, sprg1", arg0 );
					break;
				}

				case 318: {
					snprintf( buffer, BUFFER_SIZE, "mtpsr            r%ld, lpcr", arg0 );
					break;
				}

				case 319: {
					snprintf( buffer, BUFFER_SIZE, "mtpsr            r%ld, lpidr", arg0 );
					break;
				}

				case 1017: {
					snprintf( buffer, BUFFER_SIZE, "mtspr            r%ld, hid6", arg0 );
					break;
				}

				default: {
					snprintf( buffer, BUFFER_SIZE, "mtspr            r%ld, %ld", arg0, arg1 );
					break;
				}
			}
			break;
		}

		case OR: {
			snprintf( buffer, BUFFER_SIZE, "or               r%ld, r%ld, r%ld", arg0, arg1, arg2 );
			break;
		}

		case OR_IMM: {
			snprintf( buffer, BUFFER_SIZE, "or_imm           r%ld, r%ld, 0x%lx", arg0, arg1, arg2 );
			break;
		}

		case RFID: {
			snprintf( buffer, BUFFER_SIZE, "rfid" );
			break;
		}

		case SLBIA: {
			snprintf( buffer, BUFFER_SIZE, "slbia" );
			break;
		}

		case SLBMTE: {
			snprintf( buffer, BUFFER_SIZE, "slbmte           r%ld, r%ld", arg0, arg1 );
			break;
		}

		case SLD_IMM: {
			snprintf( buffer, BUFFER_SIZE, "sld_imm          r%ld, r%ld, %ld", arg0, arg1, arg2 );
			break;
		}

		case SLW: {
			snprintf( buffer, BUFFER_SIZE, "slw              r%ld, r%ld, r%ld", arg0, arg1, arg2 );
			break;
		}

		case SLW_IMM: {
			snprintf( buffer, BUFFER_SIZE, "slw_imm          r%ld, r%ld, %ld", arg0, arg1, arg2 );
			break;
		}

		case STD: {
			snprintf( buffer, BUFFER_SIZE, "std              r%ld, r%ld", arg0, arg1 );
			break;
		}

		case STD_OFF: {
			snprintf( buffer, BUFFER_SIZE, "std_off          r%ld, %ld(r%ld)", arg0, arg1, arg2 );
			break;
		}

		case STWC: {
			snprintf( buffer, BUFFER_SIZE, "stwc             r%ld, r%ld", arg0, arg1 );
			break;
		}

		case STWCX: {
			snprintf( buffer, BUFFER_SIZE, "stwcx            r%ld, r%ld, r%ld", arg0, arg1, arg2 );
			break;
		}

		case SUBF: {
			snprintf( buffer, BUFFER_SIZE, "subf             r%ld, r%ld, r%ld", arg0, arg1, arg2 );
			break;
		}

		case SYNC: {
			snprintf( buffer, BUFFER_SIZE, "sync             %ld", arg0 );
			break;
		}

		case TLBIEL: {
			snprintf( buffer, BUFFER_SIZE, "tlbiel           r%ld", arg0 );
			break;
		}
	}

	return buffer;
}

} //namespace gen7

