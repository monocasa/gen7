ADD_DEFINITIONS( "-std=c++0x" )

ADD_EXECUTABLE( testjit powerpc/TestPowerPCDisasm.cpp
                        powerpc/TestPowerPCIntermediateBuilder.cpp
                        powerpc/TestXenonCpuContext.cpp
                        TestCpuInterpreter.cpp
                        TestInterInstr.cpp )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libjit/include" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

TARGET_LINK_LIBRARIES( testjit jit gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} )

ADD_TEST( NAME testjit COMMAND "${EXECUTABLE_OUTPUT_PATH}/testjit" )

