ADD_DEFINITIONS( "-std=c++0x" )

ADD_EXECUTABLE( testjit powerpc/TestPowerPCDisasm.cpp
                        Main.cpp
                        TestCpuInterpreter.cpp
                        TestInterInstr.cpp )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libjit/include" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}" )

TARGET_LINK_LIBRARIES( testjit ${GTEST_LIBRARIES} jit ${CMAKE_THREAD_LIBS_INIT} )

