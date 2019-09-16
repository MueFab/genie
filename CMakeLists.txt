#==============================================================================
# Generic
#==============================================================================

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project("genie")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Using fall-back build type '${CMAKE_BUILD_TYPE}'")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)


#==============================================================================
# Compiler
#==============================================================================

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
else()
    message(FATAL_ERROR "C++ compiler currently not supported by CMake build")
endif()

#==============================================================================
# Options
#==============================================================================

option(BUILD_COVERAGE "Compile and link code instrumented for coverage analysis" OFF)
option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(GABAC_BUILD_SHARED_LIB "Build shared GABAC library" OFF)
option(GENIE_USE_OPENMP "Use OpenMP parallelization in Genie" OFF)

message(STATUS "Options:")
message(STATUS "  BUILD_COVERAGE=${BUILD_COVERAGE}")
message(STATUS "  BUILD_DOCUMENTATION=${BUILD_DOCUMENTATION}")
message(STATUS "  BUILD_TESTS=${BUILD_TESTS}")
message(STATUS "  GABAC_BUILD_SHARED_LIB=${GABAC_BUILD_SHARED_LIB}")
message(STATUS "  GENIE_USE_OPENMP=${GENIE_USE_OPENMP}")

if(${BUILD_COVERAGE})
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    endif()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    endif()
endif()

if(${BUILD_DOCUMENTATION})
    include(Doxygen)
endif()

if(${BUILD_TESTS})
    include(GoogleTest)
endif()

if(${GENIE_USE_OPENMP})
    find_package(OpenMP REQUIRED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    add_compile_definitions(GENIE_USE_OPENMP)
endif()


#==============================================================================
# Executable/library paths
#==============================================================================

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


#==============================================================================
# Include paths
#==============================================================================

include_directories("${CMAKE_SOURCE_DIR}/source/apps")
include_directories("${CMAKE_SOURCE_DIR}/source/libs")
include_directories("${CMAKE_SOURCE_DIR}/source/libs/gabac")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/source/libs/third-party")


#==============================================================================
# Subdirectories
#==============================================================================

add_subdirectory(source)

if(${BUILD_TESTS})
    add_subdirectory(tests)
endif()
