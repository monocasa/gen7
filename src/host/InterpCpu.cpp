#include "InterpCpu.h"

namespace Gen7 {

void InterpCpu::WriteSpr( int spr, uint64_t value )
{
	switch( spr ) {
		case 26: {
			printf( "SRR0 set to %016lx\n", value );
			srr0 = value;
			break;
		}

		case 27: {
			printf( "SRR1 set to %016lx\n", value );
			srr1 = value;
			break;
		}

		case 272: {
			printf( "Setting SPRG0 to %016lx\n", value );
			sprg0 = value;
			break;
		}

		case 273: {
			printf( "Setting SPRG1 to %016lx\n", value );
			sprg1 = value;
			break;
		}

		case 313: {
			printf( "Setting HRMOR to %016lx\n", value );
			mem.SetHrmor( value );
			break;
		}

		case 318: {
			printf( "LPCR set to %016lx\n", value );
			lpidr = value;
			break;
		}

		case 319: {
			printf( "LPIDR set to %016lx\n", value );
			lpidr = value;
			break;
		}

		case 1017: {
			printf( "HID6 set to %08lx\n", value );
			hid6 = value;
			break;
		}

		default: {
			throw Sys::Exception( "Write to unknown spr %d <- 0x%lx", spr, value );
		}
	}
}

uint64_t InterpCpu::ReadSpr( int spr  )
{
	switch( spr ) {

		case 272: {
			return sprg0;
		}

		case 273: {
			return sprg1;
		}

		case 318: {
			return lpcr;
		}

		case 1017: {
			return hid6;
		}

		case 1023: {
			return threadNum;
		}

		default: {
			throw Sys::Exception( "Read from spr %d", spr );
		}
	}
}

uint64_t InterpCpu::ReadMsr()
{
	uint64_t msr = 0;

	if( !is32Bit ) {
		msr |= 0x8000000000000000UL;
	}

	if( isHypervisor ) {
		msr |= 0x1000000000000000UL;
	}

	if( isVectorAvailable ) {
		msr |= 0x2000000;
	}

	if( isExternalInterruptEnabled ) {
		msr |= 0x8000;
	}

	if( isProblem ) {
		msr |= 0x4000;
	}

	if( isFloatingAvailable ) {
		msr |= 0x2000;
	}

	if( isMachineCheckEnabled ) {
		msr |= 0x1000;
	}

	if( isFloatingExceptionMode0 ) {
		msr |= 0x800;
	}

	if( isSingleStepEnabled ) {
		msr |= 0x400;
	}

	if( isBranchTraceEnabled ) {
		msr |= 0x200;
	}

	if( isFloatingExceptionMode1 ) {
		msr |= 0x100;
	}

	if( instructionRelocate ) {
		msr |= 0x20;
	}

	if( dataRelocate ) {
		msr |= 0x10;
	}

	if( isPerformanceMonitorMark ) {
		msr |= 0x4;
	}

	if( isRecoverableInterrupt ) {
		msr |= 0x2;
	}

	if( isLittleEndian ) {
		msr |= 0x1;
	}

	return msr;
}

void InterpCpu::WriteMsr( uint64_t newMsr )
{
	is32Bit = (newMsr & 0x8000000000000000UL) == 0;
	isHypervisor = newMsr & 0x1000000000000000UL;
	isVectorAvailable = newMsr & 0x2000000;
	isExternalInterruptEnabled = newMsr & 0x8000;
	isProblem = newMsr & 0x4000;
	isFloatingAvailable = newMsr & 0x2000;
	isMachineCheckEnabled = newMsr & 0x1000;
	isFloatingExceptionMode0 = newMsr & 0x800;
	isSingleStepEnabled = newMsr & 0x400;
	isBranchTraceEnabled = newMsr & 0x200;
	isFloatingExceptionMode1 = newMsr & 0x100;
	instructionRelocate = newMsr & 0x20;
	dataRelocate = newMsr & 0x10;
	isPerformanceMonitorMark = newMsr & 0x4;
	isRecoverableInterrupt = newMsr & 0x2;
	isLittleEndian = newMsr & 0x1;

	mem.SetInstructionRelocate( instructionRelocate );
	mem.SetIs32Bit( is32Bit );
}

void InterpCpu::InstructionStorageInterrupt()
{
	printf( "ISI %08lx\n", pc );
	srr0 = pc;
	srr1 = ReadMsr();

	uint64_t newMsr = srr1;
	newMsr &= 0xFFFFFFFFFFFFFFCF;  // Mask off instruction and data relocate

	WriteMsr( newMsr );

	pc = 0x400;
}

void InterpCpu::ExecuteTrace( Trace *trace )
{
	if( 0 == trace->instrs.size() ) {
		throw Sys::Exception( "No IR instructions in trace %p at %08lx", trace, trace->pc );
	}

	pc = trace->finalPc;

	printf( "~~Trace Start~~~\n" );

	for( const IrInstruction& instr : trace->instrs ) {

		printf( "%08lx | %s\n", instr.addr, instr.AsString().c_str() );

		switch( instr.op ) {

			case IrInstruction::ADD: {
				int targetReg = instr.arg0;
				int sourceReg1 = instr.arg1;
				int sourceReg2 = instr.arg2;

				gpr[ targetReg ] = gpr[ sourceReg1 ] + gpr[ sourceReg2 ];

				break;
			}

			case IrInstruction::ADD_IMM: {
				int targetReg = instr.arg0;
				int sourceReg = instr.arg1;
				uint64_t immediate = instr.arg2;

				gpr[ targetReg ] = gpr[ sourceReg ] + immediate;

				break;
			}

			case IrInstruction::AND_IMM: {
				int targetReg = instr.arg0;
				int sourceReg = instr.arg1;
				uint64_t immediate = instr.arg2;

				gpr[ targetReg ] = gpr[ sourceReg ] & immediate;

				break;
			}

			case IrInstruction::ANDC: {
				int targetReg = instr.arg0;
				int sourceReg1 = instr.arg1;
				int sourceReg2 = instr.arg2;

				gpr[ targetReg ] = gpr[ sourceReg1 ] & ~gpr[ sourceReg2 ];

				break;
			}

			case IrInstruction::BDNZ: {
				uint64_t target = instr.arg0;

				ctr--;

				if( 0 != ctr ) {
					pc = target;
				}

				break;
			}

			case IrInstruction::BEQ: {
				uint64_t target = instr.arg0;

				if( 0 != (0x2 & cr[0]) ) {
					pc = target;
				}

				break;
			}

			case IrInstruction::BLR: {
				if( is32Bit ) {
					pc = lr & 0x00000000FFFFFFFFUL;
				}
				else {
					pc = lr;
				}

				break;
			}

			case IrInstruction::BNE: {
				uint64_t target = instr.arg0;

				if( 0 == (0x2 & cr[0]) ) {
					pc = target;
				}

				break;
			}

			case IrInstruction::BRANCH: {
				uint64_t target = instr.arg0;

				pc = target;

				break;
			}

			case IrInstruction::BRANCH_LINK: {
				uint64_t target = instr.arg0;
				uint64_t linkTarget = instr.addr + sizeof(uint32_t);

				pc = target;
				lr = linkTarget;

				break;
			}

			case IrInstruction::CMPLW_IMM: {
				uint32_t regValue = gpr[ instr.arg0 ];
				uint32_t immValue = instr.arg1;

				if( regValue < immValue ) {
					cr[0] = 0x8;
				}
				else if( regValue > immValue ) {
					cr[0] = 0x4;
				}
				else {
					cr[0] = 0x2;
				}

				break;
			}

			case IrInstruction::EIEIO: {
				printf( "eieio\n" );

				break;
			}

			case IrInstruction::ISYNC: {
				// NOP

				break;
			}

			case IrInstruction::LOAD_IMM: {
				int targetReg = instr.arg0;
				uint64_t immediate = instr.arg1;	

				gpr[ targetReg ] = immediate;

				break;
			}

			case IrInstruction::LWAR: {
				int targetReg = instr.arg0;
				int sourceReg = instr.arg1;

				reserving = true;

				reservationAddr = gpr[ sourceReg ];

				gpr[ targetReg ] = mem.Read32( reservationAddr );

				break;
			}

			case IrInstruction::LWARX: {
				throw Sys::Exception( "lwarx" );
			}

			case IrInstruction::MFMSR: {
				int targetReg = instr.arg0;

				gpr[ targetReg ] = ReadMsr();

				break;
			}

			case IrInstruction::MFSPR: {
				int targetReg = instr.arg0;
				int spr = instr.arg1;

				gpr[ targetReg ] = ReadSpr( spr );

				break;
			}

			case IrInstruction::MTSPR: {
				int sourceReg = instr.arg0;
				int spr = instr.arg1;

				WriteSpr( spr, gpr[ sourceReg ] );

				break;
			}

			case IrInstruction::MTCTR: {
				int sourceReg = instr.arg0;

				ctr = gpr[ sourceReg ];

				break;
			}

			case IrInstruction::OR: {
				int targetReg = instr.arg0;
				int sourceReg1 = instr.arg1;
				int sourceReg2 = instr.arg2;

				gpr[ targetReg ] = gpr[ sourceReg1 ] | gpr[ sourceReg2 ];

				break;
			}

			case IrInstruction::OR_IMM: {
				int targetReg = instr.arg0;
				int sourceReg = instr.arg1;
				uint64_t immediate = instr.arg2;

				gpr[ targetReg ] = gpr[ sourceReg ] | immediate;

				break;
			}

			case IrInstruction::RFID: {
				printf( "Jump to loc_%08x\n", (uint32_t)srr0 );
				printf( "Current MSR = %016lx\n", ReadMsr() );
				printf( "   Next MSR = %016lx\n", srr1 );

				WriteMsr( srr1 );

				pc = srr0;

				break;
			}

			case IrInstruction::SLBIA: {
				printf( "slbia\n" );

				break;
			}

			case IrInstruction::SLBMTE: {
				printf( "slbmte\t%lx, %lx\n", gpr[ instr.arg0 ], gpr[ instr.arg1 ] );

				break;
			}

			case IrInstruction::SLD_IMM: {
				int targetReg = instr.arg0;
				int sourceReg = instr.arg1;
				int shift = instr.arg2;

				gpr[ targetReg ] = gpr[ sourceReg ] << shift;

				break;
			}

			case IrInstruction::SLW: {
				int targetReg = instr.arg0;
				int sourceReg = instr.arg1;
				int shiftReg = instr.arg2;

				uint32_t value = gpr[ sourceReg ];
				value <<= gpr[ shiftReg ];
				gpr[ targetReg ] = value;

				break;
			}

			case IrInstruction::SLW_IMM: {
				int targetReg = instr.arg0;
				int sourceReg = instr.arg1;
				int shift = instr.arg2;

				uint32_t value = gpr[ sourceReg ];
				value <<= shift;
				gpr[ targetReg ] = value;

				break;
			}

			case IrInstruction::STD: {
				int valueReg = instr.arg0;
				int addrReg = instr.arg1;

				mem.Write64( gpr[ addrReg ], gpr[ valueReg ] );

				break;
			}

			case IrInstruction::STD_OFF: {
				int valueReg = instr.arg0;
				int64_t offset = instr.arg1;
				int addrReg = instr.arg2;

				mem.Write64( gpr[ addrReg ] + offset, gpr[ valueReg ] );

				break;
			}

			case IrInstruction::STWC: {
				//TODO:  Implement XER SO

				int valueReg = instr.arg0;
				int addrReg = instr.arg1;

				printf( "addr=r%d value=r%d\n", addrReg, valueReg );

				if( reserving && reservationAddr == gpr[ addrReg ] ) {
					mem.Write32( gpr[ addrReg ], gpr[ valueReg ] );
					cr[0] = 0x02;
					reserving = false;
				}
				else {
					cr[0] = 0x0;
				}
				break;
			}

			case IrInstruction::STWCX: {
				throw Sys::Exception( "stwcx" );
			}

			case IrInstruction::SUBF: {
				int targetReg = instr.arg0;
				int sourceReg1 = instr.arg1;
				int sourceReg2 = instr.arg2;

				gpr[ targetReg ] =  gpr[ sourceReg2 ] - gpr[ sourceReg1 ];

				break;
			}

			case IrInstruction::SYNC: {
				// NOP

				break;
			}

			case IrInstruction::TLBIEL: {
				printf( "tlbiel\t0x%lx\n", gpr[ instr.arg0 ] );

				break;
			}
		}
	}
}

void InterpCpu::Execute()
{
	while( true ) {
		Trace* trace = traceCache.GetTrace( pc );
		if( nullptr == trace ) {
			if( !mem.IsMappedExecutable( pc ) ) {
				InstructionStorageInterrupt();
			}
			else {
				trace = traceBuilder.BuildTrace( pc, is32Bit );

				traceCache.AddTrace( trace );
			}
		}

		if( trace ) {
			ExecuteTrace( trace );
		}
	}
}

} //namespace Gen7

