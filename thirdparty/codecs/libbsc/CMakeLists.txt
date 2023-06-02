cmake_minimum_required(VERSION 2.9.0)
project(Libbsc)

set(INC_LIBRARIES
        )

include(apply_dependencies NO_POLICY_SCOPE)

file(GLOB_RECURSE Libbsc_sources "*.cpp" "*.c")

# default original makefile flags
add_definitions(-DLIBBSC_SORT_TRANSFORM_SUPPORT -D_LARGEFILE64_SOURCE -DBSC_ALLOW_UNALIGNED -D_FILE_OFFSET_BITS=64)

add_library(Libbsc OBJECT ${Libbsc_sources})


set_property(TARGET Libbsc PROPERTY POSITION_INDEPENDENT_CODE ON)

