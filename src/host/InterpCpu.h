#ifndef GEN7_HOST_INTERPCPU_H
#define GEN7_HOST_INTERPCPU_H

#include "Cpu.h"

namespace Gen7 {

class TableMmu
{
private:
	static const int NUM_SLB_ENTRIES = 32;

	static const uint64_t HRMOR_MASK = 0x000003FFFFFFFFFFUL;

	NativePhysicalMemory& physMem;

	uint64_t hrmor;

	bool instructionRelocate;
	bool is32Bit;

	struct PageInfoEntry {
		uint64_t target;
		uint64_t isMapped     : 1;
		uint64_t isExecutable : 1;
		uint64_t isReadable   : 1;
		uint64_t isWritable   : 1;
	};

	PageInfoEntry pageInfoEntries[ 0x10000 ];

public:
	void Reset() {
		hrmor = 0;
	}

	void SetHrmor( uint64_t value ) {
		hrmor = HRMOR_MASK & value;
	}

	uint64_t GetHrmor() {
		return hrmor;
	}

	uint32_t ReadEx32( uint64_t addr ) {
		if( instructionRelocate ) {
			throw Sys::Exception( "ReadEx32->%016lx", addr );
		}
		else {
			return physMem.ReadPhys32( addr );
		}
	}

	uint32_t Read32( uint64_t addr ) {
		return physMem.ReadPhys32( addr );
	}

	void Write32( uint64_t addr, uint32_t value ) {
		//TODO relocate
		printf( "WRITE32 %08x to %08lx\n", value, addr );
		physMem.WritePhys32( addr, value );
	}

	void Write64( uint64_t addr, uint64_t value ) {
		printf( "WRITE64 %lx to %08lx\n", value, addr );
		physMem.WritePhys64( addr, value );
	}

	bool IsMappedExecutable( uint64_t addr ) {
		if( instructionRelocate ) {
			if( !is32Bit ) {
				throw Sys::Exception( "instructionRelocate on 64bit not supported" );
			}
			else {
				PageInfoEntry* entry = &pageInfoEntries[ ((uint32_t)addr) / 0x10000 ];
				if( !entry->isMapped ) {
					return false;
				}
				if( !entry->isExecutable ) {
					return false;
				}
				return true;
			}
		}
		else {
			return true;
		}
	}

	void SetInstructionRelocate( bool ir ) {
		instructionRelocate = ir;
	}

	void SetIs32Bit( bool is32 ) {
		is32Bit = is32;
	}

	TableMmu( NativePhysicalMemory &physMem )
	  : physMem( physMem )
	  , hrmor( 0 )
	  , instructionRelocate( false )
	  , is32Bit( false )
	{
		for( PageInfoEntry& entry : pageInfoEntries ) {
			entry.isMapped = false;
		}
	}
};

class InterpCpu : public Cpu<TableMmu>
{
private:
	int threadNum;

	uint8_t cr[8];

	bool is32Bit;
	bool isHypervisor;
	bool isProblem;
	bool isVectorAvailable;
	bool isExternalInterruptEnabled;
	bool isFloatingAvailable;
	bool isMachineCheckEnabled;
	bool isFloatingExceptionMode0;
	bool isSingleStepEnabled;
	bool isBranchTraceEnabled;
	bool isFloatingExceptionMode1;
	bool instructionRelocate;
	bool dataRelocate;
	bool isPerformanceMonitorMark;
	bool isRecoverableInterrupt;
	bool isLittleEndian;

	enum State {
		HYPERVISOR,
		SUPERVISOR,
		PROBLEM
	};

	State getState() const {
		if( isHypervisor ) {
			return HYPERVISOR;
		}
		else if( !isProblem ) {
			return SUPERVISOR;
		}
		else {
			return PROBLEM;
		}
	}

	bool reserving;
	uint64_t reservationAddr;

	void WriteSpr( int spr, uint64_t value );
	uint64_t ReadSpr( int spr );

	void WriteMsr( uint64_t msr );

	void ExecuteTrace( Trace *trace );

	void InstructionStorageInterrupt();

protected:
	virtual uint64_t ReadMsr();

public:
	virtual void Execute();

	InterpCpu( NativePhysicalMemory& physMem, int threadNum )
	  : Cpu( physMem )
	  , threadNum( threadNum )
	  , is32Bit( false )
	  , isHypervisor( true )
	  , isProblem( false )
	  , isVectorAvailable( false )
	  , isExternalInterruptEnabled( false )
	  , isFloatingAvailable( false )
	  , isMachineCheckEnabled( true )
	  , isFloatingExceptionMode0( false )
	  , isSingleStepEnabled( false )
	  , isBranchTraceEnabled( false )
	  , isFloatingExceptionMode1( false )
	  , instructionRelocate( false )
	  , dataRelocate( false )
	  , isPerformanceMonitorMark( false )
	  , isRecoverableInterrupt( false )
	  , isLittleEndian( false )
	  , reserving( false )
	  , reservationAddr( 0 )
	{ }
};

} //namespace Gen7

#endif //GEN7_HOST_INTERPCPU_H

