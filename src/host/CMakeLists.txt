ADD_DEFINITIONS( -g -Wall -Wextra -Werror -std=c++0x )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/host" )

ADD_EXECUTABLE( gen7 crossvmm/CrossVmm.cpp
                     crossvmm/CrossVmmPhysicalMemory.cpp
                     crossvmm/KvmContext.cpp
                     sys/Mem.cpp
                     Cpu.cpp
                     ElfLoader.cpp
                     InterpCpu.cpp
                     MachineContext.cpp
                     Main.cpp
                     XenonPhysicalMemory.cpp )

