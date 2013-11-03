SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x -mno-red-zone -Wall -Wextra -Werror" )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/libjit/include" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}" )

ADD_LIBRARY( jit STATIC source/powerpc/PowerPCDisasm.cpp
                        source/Jit.cpp )

