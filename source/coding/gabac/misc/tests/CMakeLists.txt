#==============================================================================
# Gabac Tests
#==============================================================================

#==============================================================================
# Project-wide Settings
#==============================================================================

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project("tests")

# C and C++ code shall be compliant to C(++)11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

#==============================================================================
# Compiler
#==============================================================================

message(STATUS "CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
else()
    message(FATAL_ERROR "C++ compiler currently not supported by CMake build")
endif()

#==============================================================================
# Build type
#==============================================================================

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not specified")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Using fall-back build type '${CMAKE_BUILD_TYPE}'")
endif()

# Log the compiler flags for each build type
message(STATUS "C++ compiler flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Additional C++ compiler flags for build type 'Debug': ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Additional C++ compiler flags for build type 'Release': ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Additional C++ compiler flags for build type 'RelWithDebInfo': ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "Additional C++ compiler flags for build type 'MinSizeRel': ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message(STATUS "Selected build type: ${CMAKE_BUILD_TYPE}")


#==============================================================================
# Google Test
#==============================================================================

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
)
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
)
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines the gtest and gtest_main
# targets.
add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL
)

# The gtest/gtest_main targets carry header search path dependencies
# automatically when using CMake 2.8.11 or later. Otherwise we have to add
# them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

#==============================================================================
# tests
#==============================================================================

# Set the paths
set(tests "tests")
set(tests_source_dir ${PROJECT_SOURCE_DIR})
set(tests_header_dir ${PROJECT_SOURCE_DIR})
get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
set(tests_include_dir ${PARENT_DIR})


# List all source files (alphabetically)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/example_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/main.cpp)
#
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/binary_arithmetic_decoder_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/bit_input_stream_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/bit_output_stream_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/core_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/diff_coding_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/equality_coding_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/lut_transform_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/match_coding_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/rle_coding_test.cpp)
set(tests_source_files ${tests_source_files} ${tests_source_dir}/gabac/test_common.cpp)

# List all header files (alphabetically)
set(tests_header_files ${tests_header_files} ${tests_header_dir}/gabac/test_common.h)

# Group the source and header files
source_group("Tests Source Files" FILES ${tests_source_files})
source_group("Tests Header Files" FILES ${tests_header_files})

# Set up the target
add_executable(${tests} EXCLUDE_FROM_ALL ${tests_source_files})
get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
set(gabac_include_dir ${PARENT_DIR}/source/)
target_include_directories(${tests} PRIVATE ${gabac_include_dir})
target_link_libraries(${tests} gtest_main)
target_link_libraries(${tests} gabac)
