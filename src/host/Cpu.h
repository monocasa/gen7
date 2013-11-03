#ifndef GEN7_HOST_CPU_H
#define GEN7_HOST_CPU_H

#include "sys/Exception.h"
#include "PhysicalMemory.h"

#include "shared/Util.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace gen7 {

struct PowerPCInstr
{
	uint32_t opcode;

	int OPCD() const {
		return (opcode >> 26) & 0x3F;
	}

	// B-Form
	int B_BO() const {
		return (opcode >> 21) & 0x1F;
	}

	int B_BI() const {
		return (opcode >> 16) & 0x1F;
	}

	int64_t B_BD() const {
		return util::SignExtend<int64_t, 16>( opcode & 0xFFFC );
	}

	bool B_AA() const {
		return 0 != (opcode & 0x2 );
	}

	bool B_LK() const {
		return 0 != (opcode & 0x1 );
	}

	// D-Form
	int D_BF() const {
		return (opcode >> 22) & 0x7;
	}

	int D_RT() const {
		return (opcode >> 21) & 0x1F;
	}

	int D_RS() const {
		return (opcode >> 21) & 0x1F;
	}

	bool D_L() const {
		return ((opcode >> 20) & 1) != 0;
	}

	int D_RA() const {
		return (opcode >> 16) & 0x1F;
	}

	uint32_t D_UI() const {
		return opcode & 0xFFFF;
	}

	// DS-Form
	int DS_RS() const {
		return (opcode >> 21) & 0x1F;
	}

	int DS_RA() const {
		return (opcode >> 16) & 0x1F;
	}

	int DS_DS() const {
		return opcode & 0xFFFC;
	}

	int DS_XO() const {
		return opcode & 3;
	}

	// M-Form
	int M_RS() const {
		return (opcode >> 21) & 0x1F;
	}

	int M_RA() const {
		return (opcode >> 16) & 0x1F;
	}

	int M_SH() const {
		return (opcode >> 11) & 0x1F;
	}

	int M_MB() const {
		return (opcode >> 6) & 0x1F;
	}

	int M_ME() const {
		return (opcode >> 1) & 0x1F;
	}

	bool M_RC() const {
		return (opcode & 1) != 0;
	}

	// MD-Form
	int MD_RS() const {
		return (opcode >> 21) & 0x1F;
	}

	int MD_RA() const {
		return (opcode >> 16) & 0x1F;
	}

	int MD_SH() const {
		return ((opcode & 2) << 4) | ((opcode >> 11) & 0x1F);
	}

	int MD_ME() const {
		return (opcode & 0x20) | ((opcode >> 6) & 0x1F);
	}
  
	bool MD_RC() const {
		return (opcode & 1) != 0;
	}

	// MDS-Form
	int MDS_XO() const {
		return (opcode >> 1) & 0xF;
	}

	// I-Form
	int32_t I_LI() const {
		return util::SignExtend<int32_t, 26>( opcode & 0x3FFFFFC );
	}

	bool I_AA() const {
		return 0 != (opcode & 0x2 );
	}

	bool I_LK() const {
		return 0 != (opcode & 0x1 );
	}

	// X-Form
	int X_RT() const {
		return (opcode >> 21) & 0x1F;
	}

	int X_RS() const {
		return (opcode >> 21) & 0x1F;
	}

	int X_L() const {
		return (opcode >> 21) & 3;
	}

	int X_RA() const {
		return (opcode >> 16) & 0x1F;
	}

	int X_RB() const {
		return (opcode >> 11) & 0x1F;
	}

	int X_XO() const {
		return (opcode >> 1) & 0x3FF;
	}

	bool X_RC() const {
		return (opcode & 1) != 0;
	}

	// XL-Form
	int XL_BO() const {
		return (opcode >> 21) & 0x1F;
	}

	int XL_BI() const {
		return (opcode >> 16) & 0x1F;
	}

	int XL_BH() const {
		return (opcode >> 11) & 0x3;
	}

	// XFX-Form
	int XFX_RT() const {
		return (opcode >> 21) & 0x1F;
	}

	int XFX_SPR() const {
		return ((opcode >> 16) & 0x1F) | ((opcode >> 6) & 0x3E0);
	}

	// XO-Form
	int XO_RT() const {
		return (opcode >> 21) & 0x1F;
	}

	int XO_RA() const {
		return (opcode >> 16) & 0x1F;
	}

	int XO_RB() const {
		return (opcode >> 11) & 0x1F;
	}

	bool XO_OE() const {
		return (opcode & 0x400) != 0;
	}

	bool XO_RC() const {
		return (opcode & 1) != 0;
	}

	PowerPCInstr( uint32_t opcode )
	  : opcode( opcode )
	{ }
};


struct IrInstruction {
	uint64_t addr;

	enum Op {
		ADD,
		ADD_IMM,
		AND_IMM,
		ANDC,
		BDNZ,
		BEQ,
		BLR,
		BNE,
		BRANCH,
		BRANCH_LINK,
		CMPLW_IMM,
		EIEIO,
		ISYNC,
		LOAD_IMM,
		LWAR,
		LWARX,
		MFMSR,
		MFSPR,
		MTCTR,
		MTSPR,
		OR,
		OR_IMM,
		RFID,
		SLBIA,
		SLBMTE,
		SLD_IMM,
		SLW,
		SLW_IMM,
		STD,
		STD_OFF,
		STWC,
		STWCX,
		SUBF,
		SYNC,
		TLBIEL,
	} op;

	uint64_t arg0;
	uint64_t arg1;
	uint64_t arg2;

	std::string AsString() const;

	IrInstruction( uint64_t addr, Op op, uint64_t arg0 = 0, uint64_t arg1 = 0, uint64_t arg2 = 0 )
	  : addr( addr )
	  , op( op )
	  , arg0( arg0 )
	  , arg1( arg1 )
	  , arg2( arg2 )
	{ }
};


template<class MmuStrategy>
class Cpu
{
private:
	static const uint64_t BOOT_ENTRY = 0x100;

protected:
	struct Trace {
		std::vector<IrInstruction> instrs;
		uint64_t pc;
		uint64_t finalPc;
	};

	class TraceBuilder
	{
	private:
		MmuStrategy& mem;

		void AppendInstrsForOpcode( PowerPCInstr op, uint64_t pc, Trace *trace, bool is32Bit ) {
			switch( op.OPCD() ) {
				case 10: {
					if( 0 == op.D_BF() && 0 == op.D_L() ) {
						trace->instrs.push_back( IrInstruction( pc, IrInstruction::CMPLW_IMM,
						                         op.D_RA(), op.D_UI() ) );
					}
					else {
						throw Sys::Exception( "Implement cmpli %d, %d, r%d, 0x%x", op.D_BF(), op.D_L(), op.D_RA(), op.D_UI() );
					}
					return;
				}

				case 14: {
					uint64_t imm = util::SignExtend<int64_t, 16>( op.D_UI() );

					if( 0 == op.D_RA() ) {
						trace->instrs.push_back( IrInstruction( pc, IrInstruction::LOAD_IMM, 
						                         op.D_RT(), imm ) );
					}
					else {
						trace->instrs.push_back( IrInstruction( pc, IrInstruction::ADD_IMM,
						                         op.D_RT(), op.D_RA(), imm ) );
					}
					return;
				}

				case 15: {
					uint64_t imm = util::SignExtend<int64_t, 32>( op.D_UI() << 16 );

					if( 0 == op.D_RA() ) {
						trace->instrs.push_back( IrInstruction( pc, IrInstruction::LOAD_IMM, 
						                         op.D_RT(), imm ) );
					}
					else {
						trace->instrs.push_back( IrInstruction( pc, IrInstruction::ADD_IMM,
						                         op.D_RT(), op.D_RA(), imm ) );
					}
					return;
				}

				case 16: {
					uint64_t target;
					if( op.B_LK() ) {
						Sys::Exception( "Implement link bcc" );
					}

					if( op.B_AA() ) {
						target = op.B_BD();
					}
					else {
						target = op.B_BD() + pc;
					}

					if( is32Bit ) {
						target &= 0x00000000FFFFFFFFUL;
					}

					switch( op.B_BO() ) {
						case 6: {
							if( 2 == op.B_BI() ) {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::BNE, target ) );
							}
							else {
								throw Sys::Exception( "Implement bc\t%d, %d, loc_%08lx", op.B_BO(), op.B_BI(), target );
							}
							break;
						}

						case 12: {
							if( 2 == op.B_BI() ) {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::BEQ, target ) );
							}
							else {
								throw Sys::Exception( "Implement bc\t%d, %d, loc_%08lx", op.B_BO(), op.B_BI(), target );
							}
							break;
						}

						case 25: { //bdnz+
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::BDNZ, target ) );
							break;
						}

						default: {
							throw Sys::Exception( "Implement bc\t%d, %d, loc_%08lx", op.B_BO(), op.B_BI(), target );
						}
					}

					return;
				}

				case 18: {
					IrInstruction::Op opType;
					uint64_t branchTarget;

					if( op.I_LK() ) {
						opType = IrInstruction::BRANCH_LINK;
					}
					else {
						opType = IrInstruction::BRANCH;
					}

					if( op.I_AA() ) {
						branchTarget = op.I_LI();
					}
					else {
						branchTarget = op.I_LI() + pc;
					}

					if( is32Bit ) {
						branchTarget &= 0x00000000FFFFFFFFUL;
					}

					trace->instrs.push_back( IrInstruction( pc, opType, branchTarget ) );

					return;
				}

				case 19: {
					switch( op.X_XO() ) {
						case 16: {
							if( op.XL_BO() == 20 && op.XL_BI() == 0 && op.XL_BH() == 0 ) {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::BLR ) );
							}
							else {
								throw Sys::Exception( "Implement bclr %d, %d, %d", op.XL_BO(), op.XL_BI(), op.XL_BH() );
							}
							break;
						}

						case 18: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::RFID ) );

							break;
						}

						case 150: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::ISYNC ) );

							break;
						}

						default: {
							throw Sys::Exception( "Implement table 19 instr %d (%08x @ %08lx)", op.X_XO(), op.opcode, pc );
						}
					}
					return;
				}

				case 21: {
					if( op.M_RC() ) {
						throw Sys::Exception( "Implement rliwnm." );
					}

					if( (op.M_SH() == (31 - op.M_ME())) && (0 == op.M_MB()) ) {
						trace->instrs.push_back( IrInstruction( pc, IrInstruction::SLW_IMM,
						                                        op.M_RA(), op.M_RS(), op.M_SH() ) );
					}
					else {
						throw Sys::Exception( "Implement rliwnm\tr%d, r%d, %d, %d, %d",
						                       op.M_RA(), op.M_RS(), op.M_SH(), op.M_MB(), op.M_ME() );
					}
					return;
				}

				case 24: {
					trace->instrs.push_back( IrInstruction( pc, IrInstruction::OR_IMM,
					                                        op.D_RA(), op.D_RS(), op.D_UI() ) );
					return;
				}

				case 25: {
					trace->instrs.push_back( IrInstruction( pc, IrInstruction::OR_IMM,
					                                        op.D_RA(), op.D_RS(), op.D_UI() << 16 ) );
					return;
				}

				case 30: {
					switch( op.MDS_XO() ) {
						case 0: {
							if( op.MD_RC() ) {
								throw Sys::Exception( "Implement rldicl." );
							}

							if( 0 == op.MD_SH() ) {
								uint64_t mask = 0xFFFFFFFFFFFFFFFFULL;
								mask >>= op.MD_ME();
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::AND_IMM,
								                                        op.MD_RA(), op.MD_RS(), mask ) );
							}
							else {
								throw Sys::Exception( "rldicl\tr%d, r%d, %d, %d", op.MD_RA(), op.MD_RS(), op.MD_SH(), op.MD_ME() );
							}
							break;
						}

						case 3: {
							if( op.MD_RC() ) {
								throw Sys::Exception( "Implement rldicr." );
							}

							if( op.MD_SH() == (63 - op.MD_ME()) ) {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::SLD_IMM,
								                                        op.MD_RA(), op.MD_RS(), op.MD_SH() ) );
							}
							else {
								throw Sys::Exception( "Implement rldicr" );
							}
							break;
						}

						default: {
							throw Sys::Exception( "Unknown rotate instr %d (%08x @ %08lx)", op.MDS_XO(), op.opcode, pc );
						}
					}
					return;
				}

				case 31: {
					switch( op.X_XO() ) {
						case 20: {
							if( 0 == op.X_RA() ) {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::LWAR,
								                                        op.X_RT(), op.X_RB() ) );
							}
							else {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::LWARX,
								                                        op.X_RT(), op.X_RA(), op.X_RB() ) );
							}
							break;
						}

						case 24: {
							if( op.X_RC() ) {
								throw Sys::Exception( "Implement slw." );
							}

							trace->instrs.push_back( IrInstruction( pc, IrInstruction::SLW,
							                                        op.X_RA(), op.X_RS(), op.X_RB() ) );

							break;
						}

						case 40: {
							if( op.XO_RC() ) {
								throw Sys::Exception( "Implement subf." );
							}

							if( op.XO_OE() ) {
								throw Sys::Exception( "Implement subfo" );
							}

							trace->instrs.push_back( IrInstruction( pc, IrInstruction::SUBF,
							                                        op.X_RT(), op.XO_RA(), op.XO_RB() ) );

							break;
						}

						case 60: {
							if( op.X_RC() ) {
								throw Sys::Exception( "Implement andc." );
							}

							trace->instrs.push_back( IrInstruction( pc, IrInstruction::ANDC,
							                                        op.X_RA(), op.X_RS(), op.X_RB() ) );

							break;
						}

						case 83: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::MFMSR, op.X_RT() ) );

							break;
						}

						case 150: {
							if( 0 == op.X_RA() ) {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::STWC,
								                                        op.X_RT(), op.X_RB() ) );
							}
							else {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::STWCX,
								                                        op.X_RT(), op.X_RA(), op.X_RB() ) );
							}
							break;
						}

						case 266: {
							if( op.X_RC() ) {
								throw Sys::Exception( "Implement add." );
							}

							trace->instrs.push_back( IrInstruction( pc, IrInstruction::ADD,
							                                        op.X_RA(), op.X_RS(), op.X_RB() ) );

							break;
						}

						case 274: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::TLBIEL, op.X_RB() ) );
							break;
						}

						case 339: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::MFSPR,
							                                        op.XFX_RT(), op.XFX_SPR() ) );
							break;
						}

						case 402: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::SLBMTE,
							                                        op.X_RS(), op.X_RB() ) );
							break;
						}

						case 444: {
							if( op.X_RC() ) {
								throw Sys::Exception( "Implement or." );
							}

							trace->instrs.push_back( IrInstruction( pc, IrInstruction::OR,
							                                        op.X_RA(), op.X_RS(), op.X_RB() ) );

							break;
						}

						case 467: {
							switch( op.XFX_SPR() ) {
								case 9: {
									trace->instrs.push_back( IrInstruction( pc, IrInstruction::MTCTR, op.XFX_RT() ) );
									break;
								}

								default: {
									trace->instrs.push_back( IrInstruction( pc, IrInstruction::MTSPR,
									                                        op.XFX_RT(), op.XFX_SPR() ) );
									break;
								}
							}
							break;
						}

						case 498: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::SLBIA ) );

							break;
						}

						case 598: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::SYNC, op.X_L() ) );

							break;
						}

						case 854: {
							trace->instrs.push_back( IrInstruction( pc, IrInstruction::EIEIO ) );

							break;
						}

						default: {
							throw Sys::Exception( "Implement table 31 instr %d (%08x @ %08lx)", op.X_XO(), op.opcode, pc );
						}
					}
					return;
				}

				case 62: {
					switch( op.DS_XO() ) {
						case 0: {
							int64_t offset = util::SignExtend<int64_t, 16>( op.DS_DS() );
							if( 0 == offset ) {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::STD, op.DS_RS(), op.DS_RA() ) );
							}
							else {
								trace->instrs.push_back( IrInstruction( pc, IrInstruction::STD_OFF, op.DS_RS(), offset, op.DS_RA() ) );
							}
							break;
						}

						default: {
							throw Sys::Exception( "Unimplemented table 62 instr %d (%08x @ %08lx)", op.DS_XO(), op.opcode, pc );
						}
					}
					return;
				}

				default: {
					throw Sys::Exception( "Unknown opcode %d in Cpu::TraceBuilder::AppendInstrsForOpcode( opcode=%08x, pc=%08lx, trace=%p )", op.OPCD(), op.opcode, pc, trace );
				}
			}
		}

		bool ContinueTrace( IrInstruction &instr) {
			//TODO:  We definitely don't want to continue across page boundaries

			switch( instr.op ) {
				case IrInstruction::BEQ:
				case IrInstruction::BLR:
				case IrInstruction::BNE: 
				case IrInstruction::BRANCH: 
				case IrInstruction::BRANCH_LINK:
				case IrInstruction::BDNZ:
				case IrInstruction::MTSPR:
				case IrInstruction::RFID:
				case IrInstruction::SLBIA:
				case IrInstruction::SLBMTE:
				case IrInstruction::TLBIEL: {
					return false;
				}

				default: {
					return true;
				}
			}
		}

	public:
		Trace* BuildTrace( uint64_t pc, bool is32Bit ) {
			Trace *trace = new Trace();
			trace->pc = pc;

			uint32_t opcode;

			do {
				opcode = mem.ReadEx32( pc );
				PowerPCInstr op( opcode );
				AppendInstrsForOpcode( op, pc, trace, is32Bit );
				pc += sizeof(uint32_t);
			} while( ContinueTrace( trace->instrs[trace->instrs.size() - 1] ) );

			trace->finalPc = pc;

			return trace;
		}

		TraceBuilder( MmuStrategy &mem )
		  : mem( mem )
		{ }
	};

	class TraceCache
	{
	private:
		static const int NUM_SAVED_ENTRIES = 64;

		typedef std::map<uint64_t,Trace*> TraceAddrMap;

		int currentKill;

		Trace* traces[ NUM_SAVED_ENTRIES ];
		TraceAddrMap traceAddrMap;
		
	public:
		Trace* GetTrace( uint64_t pc ) {
			typename TraceAddrMap::iterator it = traceAddrMap.find( pc );

			if( it != traceAddrMap.end() ) {
				return traceAddrMap[ pc ];
			}
			else {
				return nullptr;
			}
		}

		void AddTrace( Trace *trace ) {
			typename TraceAddrMap::iterator it = traceAddrMap.find( trace->pc );

			Trace** traceSlot;

			if( it != traceAddrMap.end() ) {
				for( int i = 0; i < NUM_SAVED_ENTRIES; i++ ) {
					if( traces[i]->pc == trace->pc ) {
						traceSlot = &traces[i];
						break;
					}
				}
				delete *traceSlot;
			}
			else {
				traceSlot = &traces[ currentKill ];
				currentKill++;
				currentKill %= NUM_SAVED_ENTRIES;

				Trace* previous = *traceSlot;
				if( nullptr != previous ) {
					delete previous;
				}
			}
			*traceSlot = trace;
		}

		TraceCache()
		  : currentKill( 0 )
		{
			for( Trace* &tracePtr : traces ) {
				tracePtr = nullptr;
			}
		}
	};

	NativePhysicalMemory &physMem;

	uint64_t gpr[64];
	uint64_t pc;

	uint64_t ctr;
	uint64_t lr;

	uint64_t srr0;
	uint64_t srr1;

	uint64_t hid6;
	uint64_t lpcr;
	uint64_t lpidr;
	uint64_t sprg0;
	uint64_t sprg1;

	MmuStrategy mem;
	TraceCache traceCache;
	TraceBuilder traceBuilder;

protected:
	virtual uint64_t ReadMsr() = 0;

public:
	void SetPc( uint64_t newPc ) {
		pc = newPc;
	}

	virtual void Reset() {
		for( uint64_t &reg : gpr ) {
			reg = 0;
		}

		pc = BOOT_ENTRY;
		lr = 0;
		ctr = 0;

		srr0 = 0;
		srr1 = 0;

		hid6 = 0; //TODO: Discover reset state of HID6
		lpcr = 0;
		lpidr = 0;
		sprg0 = 0;
		sprg1 = 0;

		mem.Reset();
	}

	void DumpState() {
		printf( " r0 %16lx |  r1 %16lx |  r2 %16lx |  r3 %16lx\n", gpr[ 0], gpr[ 1], gpr[ 2], gpr[ 3] );
		printf( " r4 %16lx |  r5 %16lx |  r6 %16lx |  r7 %16lx\n", gpr[ 4], gpr[ 5], gpr[ 6], gpr[ 7] );
		printf( " r8 %16lx |  r9 %16lx | r10 %16lx | r11 %16lx\n", gpr[ 8], gpr[ 9], gpr[10], gpr[11] );
		printf( "r12 %16lx | r13 %16lx | r14 %16lx | r15 %16lx\n", gpr[12], gpr[13], gpr[14], gpr[15] );
		printf( "r16 %16lx | r17 %16lx | r18 %16lx | r19 %16lx\n", gpr[16], gpr[17], gpr[18], gpr[19] );
		printf( "r20 %16lx | r21 %16lx | r22 %16lx | r23 %16lx\n", gpr[20], gpr[21], gpr[22], gpr[23] );
		printf( "r24 %16lx | r25 %16lx | r26 %16lx | r27 %16lx\n", gpr[24], gpr[25], gpr[26], gpr[27] );
		printf( "r28 %16lx | r29 %16lx | r30 %16lx | r31 %16lx\n", gpr[28], gpr[29], gpr[30], gpr[31] );
		printf( " pc %16lx |  lr %16lx | msr %016lx\n", pc, lr, ReadMsr() );
	}

	virtual void Execute() = 0;

	Cpu( NativePhysicalMemory& physMem )
	  : physMem( physMem )
	  , mem( physMem )
	  , traceBuilder( mem )
	{ }
};

} //namespace gen7

#endif //GEN7_HOST_CPU_H

