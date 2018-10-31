#==============================================================================
# Generic
#==============================================================================

# We require at least CMake 3.1
cmake_minimum_required(VERSION 3.1)
message(STATUS "CMake generator: ${CMAKE_GENERATOR}")

# C++ code shall be compliant to C++11
set(CMAKE_CXX_STANDARD 11)

# This is genie
project("genie")

# Custom CMake cache variables
set(GENIE_BUILD_TESTS OFF CACHE BOOL "Build unit tests (target 'tests').")
set(GENIE_DISABLE_ASSERTS OFF CACHE BOOL "Disable assert() statements.")
set(GENIE_USE_OPENMP OFF CACHE BOOL "Use OpenMP parallelization.")
message(STATUS "Custom CMake cache variables:")
message(STATUS "GENIE_BUILD_TESTS=${GENIE_BUILD_TESTS}")
message(STATUS "GENIE_DISABLE_ASSERTS=${GENIE_DISABLE_ASSERTS}")
message(STATUS "GENIE_USE_OPENMP=${GENIE_USE_OPENMP}")


#==============================================================================
# Boost
#==============================================================================

find_package(Boost 1.67 COMPONENTS filesystem log program_options system REQUIRED)


#==============================================================================
# OpenMP
#==============================================================================

if (${GENIE_USE_OPENMP})
    find_package(OpenMP REQUIRED)
endif ()

#==============================================================================
# Google Test
#==============================================================================

if (${GENIE_BUILD_TESTS})
    # Download and unpack googletest at configure time
    configure_file(CMakeListsGoogleTest.txt.in googletest/download/CMakeLists.txt)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest/download
    )
    if (result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest/download
    )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker settings on
    # Windows
    # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build and define the gtest and gtest_main
    # targets.
    add_subdirectory(
        ${CMAKE_BINARY_DIR}/googletest/src
        ${CMAKE_BINARY_DIR}/googletest/build
        EXCLUDE_FROM_ALL
    )
endif ()


#==============================================================================
# Compiler
#==============================================================================

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "C++ compiler: GNU Compiler Collection")
    message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID})")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    if (${GENIE_USE_OPENMP})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "C++ compiler: LLVM Clang or Apple Clang")
    message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID})")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
     message(STATUS "C++ compiler: Intel")
     message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID})")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "C++ compiler: Microsoft Visual Studio")
    message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID})")
else ()
    message(FATAL_ERROR "C++ compiler currently not supported by CMake build")
    message(FATAL_ERROR "Compiler ID: ${CMAKE_CXX_COMPILER_ID})")
endif ()


#==============================================================================
# Build type
#==============================================================================

# Detect the build type
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Build type: Debug")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Build type: Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Build type: RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Build type: MinSizeRel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
else ()
    message(STATUS "Build type: None (CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})")
endif ()

# Log the compiler flags for each build type
message(STATUS "C++ compiler flags for build type 'None': ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ compiler flags for build type 'Debug': ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ compiler flags for build type 'Release': ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "C++ compiler flags for build type 'RelWithDebInfo': ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "C++ compiler flags for build type 'MinSizeRel': ${CMAKE_CXX_FLAGS_MINSIZEREL}")


#==============================================================================
# genie
#==============================================================================

# Set ou genie paths
set(genie "genie")
set(genie_source_dir ${CMAKE_SOURCE_DIR}/source)
set(genie_header_dir ${CMAKE_SOURCE_DIR}/source)
set(genie_include_dir ${CMAKE_SOURCE_DIR}/source)

# List all source files
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/spring.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/preprocess.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/util.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/bitset_util.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/encoder.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/reorder_compress_quality_id.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/generate_read_streams.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/generate_new_fastq.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/decode_read_streams.cpp)
#
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fasta_file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fasta_record.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fastq_file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fastq_record.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/sam_file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/sam_record.cc)
#
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/dsg.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/exceptions.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/generation.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/ProgramOptions.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/utilities.cc)


# Group the source and header files
source_group("Source Files" FILES ${gabac_source_files})
# source_group("Header Files" FILES ${gabac_header_files})

# Set up the genie target
add_executable(${genie} ${genie_source_files})
if (${GENIE_DISABLE_ASSERTS})
    target_compile_definitions(${genie} PRIVATE NDEBUG)
endif ()
if (${GENIE_USE_OPENMP})
    target_compile_definitions(${genie} PRIVATE GENIE_USE_OPENMP)
endif ()
target_include_directories(${genie} PRIVATE ${genie_include_dir})
target_link_libraries(${genie} Boost::program_options)
target_link_libraries(${genie} Boost::filesystem)
target_link_libraries(${genie} Boost::system)


#==============================================================================
# tests
#==============================================================================

if (${GENIE_BUILD_TESTS})
    # Set our tests paths
    set(tests "tests")
    set(tests_source_dir ${CMAKE_SOURCE_DIR}/tests)
    set(tests_header_dir ${CMAKE_SOURCE_DIR}/tests)
    set(tests_include_dir ${CMAKE_SOURCE_DIR})

    # List all source files (alphabetically)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/format/example_test.cpp)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/main.cpp)

    # List all header files (alphabetically)
    # set(tests_header_files ${tests_header_files} ${tests_header_dir}/whatever.h)

    # Group the source and header files
    source_group("Source Files" FILES ${tests_source_files})
    # source_group("Header Files" FILES ${tests_header_files})

    # Set up the tests target
    add_executable(${tests} EXCLUDE_FROM_ALL ${tests_source_files})
    target_include_directories(${tests} PRIVATE ${CMAKE_BINARY_DIR})
    target_link_libraries(${tests} gtest_main)
endif ()
