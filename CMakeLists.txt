#==============================================================================
# Generic
#==============================================================================

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project("genie")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


#==============================================================================
# Build type
#==============================================================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Using fall-back build type: ${CMAKE_BUILD_TYPE}")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    #
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    #
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(WARNING "Build type not supported: ${CMAKE_BUILD_TYPE}")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(WARNING "Build type not supported: ${CMAKE_BUILD_TYPE}")
else()
    message(WARNING "Build type not supported: ${CMAKE_BUILD_TYPE}")
endif()


#==============================================================================
# Compiler
#==============================================================================

set(CMAKE_CXX_STANDARD 11)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # GNU Compiler Collection
    #
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    #
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # Apple Clang or LLVM Clang
    #
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    #
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel") # Intel Compiler
    #
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    #
else()
    message(WARNING "C++ compiler not supported: ${CMAKE_CXX_COMPILER_ID}")
endif()


#==============================================================================
# Options
#==============================================================================

option(BUILD_COVERAGE "Compile and link code instrumented for coverage analysis" OFF)
option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(GABAC_BUILD_SHARED_LIB "Build shared GABAC library" OFF)
option(GENIE_USE_OPENMP "Use OpenMP parallelization in Genie" OFF)

message(STATUS "")
message(STATUS "******** Options ********")
message(STATUS "  BUILD_COVERAGE         : ${BUILD_COVERAGE}")
message(STATUS "  BUILD_DOCUMENTATION    : ${BUILD_DOCUMENTATION}")
message(STATUS "  BUILD_TESTS            : ${BUILD_TESTS}")
message(STATUS "  GABAC_BUILD_SHARED_LIB : ${GABAC_BUILD_SHARED_LIB}")
message(STATUS "  GENIE_USE_OPENMP       : ${GENIE_USE_OPENMP}")
message(STATUS "")

if(${BUILD_COVERAGE})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    else()
        message(FATAL_ERROR "Coverage analysis not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

if(${BUILD_DOCUMENTATION})
    include(Doxygen)
endif()

if(${BUILD_TESTS})
    include(GoogleTest)
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

include_directories("${CMAKE_SOURCE_DIR}/src/apps")
include_directories("${CMAKE_SOURCE_DIR}/src/libs")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/thirdparty")


#==============================================================================
# Subdirectories
#==============================================================================

add_subdirectory(src)

if(${BUILD_TESTS})
    add_subdirectory(test)
endif()


#==============================================================================
# Summary
#==============================================================================

include(Summary)
print_summary()
