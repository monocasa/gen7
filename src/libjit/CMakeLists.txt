INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libjit/include" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}" )

SET( LIBJIT_SOURCES source/powerpc/PowerPCDisasm.cpp
                    source/powerpc/PowerPCIntermediateBuilder.cpp
                    source/CpuInterpreter.cpp )

ADD_LIBRARY( jit STATIC ${LIBJIT_SOURCES} )

SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-m64 -mcmodel=kernel -std=c++0x -D__64BIT__ -mno-red-zone -fno-rtti -fno-exceptions -fno-stack-protector -Wall -Wextra -Werror" )

ADD_LIBRARY( jit-vmm STATIC ${LIBJIT_SOURCES} )

