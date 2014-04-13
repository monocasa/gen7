ADD_DEFINITIONS( "-std=c++0x" )

ADD_EXECUTABLE( testutil test/TestBitOps.cpp )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

TARGET_LINK_LIBRARIES( testutil util gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} )

ADD_TEST( NAME testutil COMMAND "${EXECUTABLE_OUTPUT_PATH}/testjit" )

