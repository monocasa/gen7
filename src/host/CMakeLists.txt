ADD_DEFINITIONS( -g -Wall -Wextra -Werror -std=c++0x )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/host" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libjit/include" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

ADD_LIBRARY( gen7 SHARED crossvmm/CrossVmm.cpp
                         crossvmm/CrossVmmPhysicalMemory.cpp
                         crossvmm/KvmContext.cpp
                         dc/DcMachineContext.cpp
                         dc/DcPhysicalMemory.cpp
                         ps3/Ps3PhysicalMemory.cpp
                         sys/Mem.cpp
                         xenon/XenonMachineContext.cpp
                         xenon/XenonPhysicalMemory.cpp
                         Cpu.cpp
                         ElfLoader.cpp
                         InterpCpu.cpp
                         MachineContext.cpp
                         Subsystem.cpp )

TARGET_LINK_LIBRARIES( gen7 ${Boost_LIBRARIES} )

