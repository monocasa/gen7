INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libjit/include" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

SET( LIBJIT_SOURCES source/powerpc/PowerPCDisasm.cpp
                    source/powerpc/PowerPCIntermediateBuilder.cpp )

ADD_LIBRARY( jit STATIC ${LIBJIT_SOURCES} )

ADD_EXECUTABLE( test_jit test/powerpc/TestPowerPCDisasm.cpp
                         test/powerpc/TestPowerPCIntermediateBuilder.cpp
                         test/powerpc/TestXenonCpuContext.cpp
                         test/TestCpuInterpreter.cpp
                         test/TestInterInstr.cpp )

TARGET_LINK_LIBRARIES( test_jit jit gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} )

ADD_TEST( NAME test_jit COMMAND "${EXECUTABLE_OUTPUT_PATH}/test_jit" )

SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-m64 -mcmodel=kernel -std=c++0x -D__64BIT__ -mno-red-zone -fno-rtti -fno-exceptions -fno-stack-protector -Wall -Wextra -Werror" )

ADD_LIBRARY( jit-vmm STATIC ${LIBJIT_SOURCES} )

