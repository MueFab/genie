#==============================================================================
# Generic
#==============================================================================

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project("genie")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(AddGoogleTest)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)


#==============================================================================
# Compiler
#==============================================================================

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")


#==============================================================================
# OpenMP
#==============================================================================

option(GENIE_USE_OPENMP "Use OpenMP parallelization" OFF)
message(STATUS "GENIE_USE_OPENMP=${GENIE_USE_OPENMP}")
if(${GENIE_USE_OPENMP})
    find_package(OpenMP REQUIRED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    add_compile_definitions(GENIE_USE_OPENMP)
endif()


#==============================================================================
# Build type
#==============================================================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Using fall-back build type '${CMAKE_BUILD_TYPE}'")
endif()


#==============================================================================
# Executable/library paths
#==============================================================================

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


#==============================================================================
# Include paths
#==============================================================================

include_directories("${CMAKE_SOURCE_DIR}/source/libs")
include_directories("${CMAKE_SOURCE_DIR}/source/libs/gabac")
include_directories("${CMAKE_SOURCE_DIR}/source/libs/third-party")


#==============================================================================
# Subdirectories
#==============================================================================

add_subdirectory(source)
add_subdirectory(tests)
