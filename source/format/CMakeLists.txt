project(mpegg_encapsulator)

file(GLOB_RECURSE SOURCE_FILES "*.c")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(mpegg-libencapsulator STATIC ${SOURCE_FILES} EncodingParameters_tests.h DataStructures/EncodingParameters.c)
set(encapsulator_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)