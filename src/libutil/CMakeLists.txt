ADD_DEFINITIONS( "-std=c++0x" )

ADD_EXECUTABLE( test_util test/TestBitOps.cpp )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

TARGET_LINK_LIBRARIES( test_util util gtest gtest_main ${CMAKE_THREAD_LIBS_INIT} )

ADD_TEST( NAME test_util COMMAND "${EXECUTABLE_OUTPUT_PATH}/test_jit" )

