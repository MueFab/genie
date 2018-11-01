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

find_package(Boost 1.58 COMPONENTS filesystem log program_options system REQUIRED)


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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
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
# format library
#==============================================================================

# Set our format paths
set(format "format")
set(format_source_dir ${CMAKE_SOURCE_DIR}/source/format)
set(format_header_dir ${CMAKE_SOURCE_DIR}/source/format)
set(format_include_dir ${CMAKE_SOURCE_DIR}/source/format ${CMAKE_SOURCE_DIR}/source)

# List all source files
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/AccessUnit/AccessUnitContainer.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/AccessUnit/AccessUnitHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/AccessUnit/AccessUnitInformation.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/AccessUnit/AccessUnitProtection.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Block/BlockContainer.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Block/BlockHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Dataset/DatasetContainer.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Dataset/DatasetHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Dataset/DatasetMasterIndexTable.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Dataset/DatasetMetadata.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Dataset/DatasetParameters.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Dataset/DatasetProtection.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/DatasetsGroup/DatasetsGroupContainer.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/DatasetsGroup/DatasetsGroupHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/DatasetsGroup/DatasetsGroupLabel.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/DatasetsGroup/DatasetsGroupMetadata.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/DatasetsGroup/DatasetsGroupProtection.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/DatasetsGroup/DatasetsGroupReferenceGenome.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/File/File.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/File/FileHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Stream/StreamContainer.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Stream/StreamHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Stream/StreamMetadata.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Stream/StreamProtection.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/BitStreams/InputBitstream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/BitStreams/OutputBitstream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/Signature.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignatureInputInteger.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignatureIntegerInputStream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignatureIntegerOutputStream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/Signatures.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignatureSizeComputation.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/ByteArray.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/EncodingParameters.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/FromFile.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Label.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataUnits/DataUnitAccessUnit.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataUnits/DataUnitBlockHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataUnits/DataUnitsParametersSet.c)
set(format_source_files ${format_source_files} ${format_source_dir}/Boxes.c)
set(format_source_files ${format_source_files} ${format_source_dir}/ClassesSet.c)
set(format_source_files ${format_source_files} ${format_source_dir}/treeUint64.c)
set(format_source_files ${format_source_files} ${format_source_dir}/utils.c)
set(format_source_files ${format_source_files} ${format_source_dir}/vector.c)
set(format_source_files ${format_source_files} ${format_source_dir}/VectorUint64.c)

# List all header files
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/BitStreams/InputBitstream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/BitStreams/OutputBitstream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/Signature.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignatureInputInteger.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignatureIntegerInputStream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignatureIntegerOutputStream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/Signatures.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignatureSizeComputation.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/FromFile.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataUnits/DataUnits.h)
set(format_header_files ${format_header_files} ${format_header_dir}/Boxes.h)
set(format_header_files ${format_header_files} ${format_header_dir}/ClassesSet.h)
set(format_header_files ${format_header_files} ${format_header_dir}/EncodingParameters.h)
set(format_header_files ${format_header_files} ${format_header_dir}/treeUint64.h)
set(format_header_files ${format_header_files} ${format_header_dir}/utils.h)
set(format_header_files ${format_header_files} ${format_header_dir}/vector.h)
set(format_header_files ${format_header_files} ${format_header_dir}/vectorUint64.h)

# Group the source and header files
source_group("Source Files" FILES ${format_source_files})
source_group("Header Files" FILES ${format_header_files})

# Set up the format target
add_library(${format} STATIC ${format_source_files})
target_include_directories(${format} PRIVATE ${format_include_dir})

#==============================================================================
# MPEGGFileCreation library
#==============================================================================

# Set our format paths
set(MPEGGFileCreation "MPEGGFileCreation")
set(MPEGGFileCreation_source_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation)
set(MPEGGFileCreation_header_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation)
set(MPEGGFileCreation_include_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation ${CMAKE_SOURCE_DIR}/source)

# List all source files
set(MPEGGFileCreation_source_files ${MPEGGFileCreation_source_files} ${MPEGGFileCreation_source_dir}/MPEGGFileCreator.cpp)


# List all header files
set(MPEGGFileCreation_header_files ${MPEGGFileCreation_header_files} ${MPEGGFileCreation_header_dir}/MPEGGFileCreator.h)


# Group the source and header files
source_group("Source Files" FILES ${format_source_files})
source_group("Header Files" FILES ${format_header_files})

# Set up the format target
add_library(${MPEGGFileCreation} STATIC ${MPEGGFileCreation_source_files})
target_include_directories(${MPEGGFileCreation} PRIVATE ${MPEGGFileCreation_include_dir})
target_link_libraries(${MPEGGFileCreation} ${format})

#==============================================================================
# genie
#==============================================================================

# Set our genie paths
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
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/main.cpp)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/format/EncodingParameters_tests.cpp)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/format/example_test.cpp)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/utils/MPEGGFileCreation/MPEGGFileCreatorTest.cpp)

    # List all header files (alphabetically)
    # set(tests_header_files ${tests_header_files} ${tests_header_dir}/whatever.h)

    # Group the source and header files
    source_group("Source Files" FILES ${tests_source_files})
    # source_group("Header Files" FILES ${tests_header_files})

    # Set up the tests target
    add_executable(${tests} EXCLUDE_FROM_ALL ${tests_source_files})
    target_include_directories(${tests} PRIVATE ${genie_include_dir})
    target_include_directories(${tests} PRIVATE ${CMAKE_BINARY_DIR})
    target_link_libraries(${tests} gtest_main)
    target_link_libraries(${tests} ${format})
    target_link_libraries(${tests} ${MPEGGFileCreation})
endif ()
