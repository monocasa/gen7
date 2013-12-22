INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libutil/include" )

SET( LIBUTIL_SOURCES source/BitOps.cpp )

ADD_LIBRARY( util STATIC ${LIBUTIL_SOURCES} )

SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-m64 -mcmodel=kernel -std=c++0x -D__64BIT__ -mno-red-zone -fno-rtti -fno-exceptions -fno-stack-protector -Wall -Wextra -Werror" )

ADD_LIBRARY( util-vmm STATIC ${LIBUTIL_SOURCES} )

