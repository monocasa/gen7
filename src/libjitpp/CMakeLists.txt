INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libjitpp/include" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

SET( LIBJIT_SOURCES source/powerpc/GekkoTranslator.cpp
                    source/powerpc/PowerPCDecoder.cpp
                    source/powerpc/PowerPCDisasm.cpp
                    source/powerpc/PowerPCIntermediateBuilder.cpp )

ADD_LIBRARY( jitpp STATIC ${LIBJIT_SOURCES} )

ADD_EXECUTABLE( test_jitpp test/powerpc/Test_GekkoTranslator.cpp
                           test/powerpc/Test_PowerPCDisasm.cpp
                           test/powerpc/Test_PowerPCIntermediateBuilder.cpp
                           test/powerpc/Test_XenonCpuContext.cpp
                           test/Test_CommonOp.cpp
                           test/Test_CommonOpInterpreter.cpp
                           test/Test_CpuInterpreter.cpp
                           test/Test_InterInstr.cpp )

TARGET_LINK_LIBRARIES( test_jitpp jitpp gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} )

ADD_TEST( NAME test_jitpp COMMAND "${EXECUTABLE_OUTPUT_PATH}/test_jitpp" )

SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-m64 -mcmodel=kernel -std=c++1y -D__64BIT__ -mno-red-zone -fno-rtti -fno-exceptions -fno-stack-protector -Wall -Wextra -Werror" )

ADD_LIBRARY( jitpp-vmm STATIC ${LIBJIT_SOURCES} )

