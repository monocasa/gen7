ADD_DEFINITIONS( -g -Wall -Wextra -Werror -std=c++0x )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/host )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/libjit/include )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/libutil/include )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

ADD_EXECUTABLE( gen7-cli Main.cpp )

TARGET_LINK_LIBRARIES( gen7-cli gen7
                                ${Boost_LIBRARIES} )

