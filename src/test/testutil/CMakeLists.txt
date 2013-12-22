ADD_DEFINITIONS( "-std=c++0x" )

ADD_EXECUTABLE( testutil Main.cpp
                         TestBitOps.cpp )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

TARGET_LINK_LIBRARIES( testutil ${GTEST_LIBRARIES} util ${CMAKE_THREAD_LIBS_INIT} )

