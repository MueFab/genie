cmake_minimum_required(VERSION 3.5)
project(bsc)

file(GLOB_RECURSE bsc_sources "*.cpp" "*.c")

# default original makefile flags
add_definitions(-DLIBBSC_SORT_TRANSFORM_SUPPORT -D_LARGEFILE64_SOURCE -DBSC_ALLOW_UNALIGNED -D_FILE_OFFSET_BITS=64)

add_library(bsc STATIC ${bsc_sources})

set_target_properties(bsc PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(bsc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
