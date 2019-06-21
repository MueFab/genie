#==============================================================================
# Generic
#==============================================================================

# We require at least CMake 3.1
cmake_minimum_required(VERSION 3.1)
message(STATUS "CMake generator: ${CMAKE_GENERATOR}")

# C++ code shall be compliant to C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# This is genie
project("genie")

# Custom CMake cache variables
set(GENIE_USE_OPENMP OFF CACHE BOOL "Use OpenMP parallelization.")
message(STATUS "Custom CMake cache variables:")
message(STATUS "GENIE_USE_OPENMP=${GENIE_USE_OPENMP}")


#==============================================================================
# OpenMP
#==============================================================================

if (${GENIE_USE_OPENMP})
    find_package(OpenMP REQUIRED)
endif ()


#==============================================================================
# Build type
#==============================================================================

# Set default build type
if (NOT CMAKE_BUILD_TYPE)
    message(WARNING "Unsupported build type (CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})")
    set(CMAKE_BUILD_TYPE Debug)
    message(WARNING "Using default build type '${CMAKE_BUILD_TYPE}'")
endif ()

# Set build type-specific flags
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

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
configure_file(CMakeListsGoogleTest.txt.in googletest/download/CMakeLists.txt)
execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest/download
)
if(result)
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
    ${CMAKE_BINARY_DIR}/googletest-src
    ${CMAKE_BINARY_DIR}/googletest-build
    EXCLUDE_FROM_ALL
)

#==============================================================================
# gabac
#==============================================================================

configure_file(CMakeLists_gabac.txt.in gabac-download/CMakeLists.txt)
set(BUILD_GABACIFY OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gabac-download
)
if(result2)
    message(FATAL_ERROR "CMake step for gabac failed: ${result2}")
endif()
execute_process(
    COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gabac-download
)
if(result2)
    message(FATAL_ERROR "Build step for gabac failed: ${result2}")
endif()

# Add gabac directly to our build. This defines the gtest and gtest_main
# targets.
add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/gabac-src
    ${CMAKE_CURRENT_BINARY_DIR}/gabac-build
    EXCLUDE_FROM_ALL
)


#==============================================================================
# Compiler
#==============================================================================

message(STATUS "CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID})")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    if (${GENIE_USE_OPENMP})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "C++ compiler: LLVM Clang or Apple Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
else ()
    message(FATAL_ERROR "C++ compiler currently not supported by CMake build")
endif ()


#==============================================================================
# coding library
#==============================================================================

# Set our coding paths
set(coding "coding")
set(coding_source_dir ${CMAKE_SOURCE_DIR}/source/coding )
set(coding_header_dir ${CMAKE_SOURCE_DIR}/source/coding )
set(coding_include_dir ${CMAKE_SOURCE_DIR}/source/coding ${CMAKE_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/gabac-src/source)

# List all source files
set(coding_source_files ${coding_source_files} ${coding_source_dir}/ReturnStructures.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/spring.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/preprocess.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/util.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/bitset_util.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/encoder.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/reorder_compress_quality_id_eru.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_read_streams_eru.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_ref_streams_eru.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_new_fastq.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/decode_eru.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/id_tokenization.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/pe_encode.cpp)

# List all header files
set(coding_source_files ${coding_source_files} ${coding_source_dir}/ReturnStructures.h)
set(coding_header_files ${coding_header_files} ${coding_header_dir}/CoderConstans.h)
set(coding_header_files ${coding_header_files} ${coding_header_dir}/spring.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/bitset_util.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/BooPHF.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/decode_eru.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/encoder.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_new_fastq.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_read_streams_eru.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_ref_streams_eru.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/id_tokenization.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/params.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/pe_encode.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/preprocess.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/reorder_compress_quality_id_eru.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/reorder.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/spring.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/util.h)

# Group the source and header files
source_group("Source Files" FILES ${coding_source_files})
source_group("Header Files" FILES ${coding_header_files})

# Set up the format target
add_library(${coding} STATIC ${coding_source_files} ${calq_source_files} ${sam_source_files})
target_include_directories(${coding} PRIVATE ${coding_include_dir} ${sam_include_dir} ${calq_source_dir})

#==============================================================================
# format library
#==============================================================================

# Set the paths
set(format "format")
set(format_source_dir ${CMAKE_SOURCE_DIR}/source/format)
set(format_header_dir ${CMAKE_SOURCE_DIR}/source/format)
set(format_include_dir ${CMAKE_SOURCE_DIR}/source/format ${CMAKE_SOURCE_DIR}/source)

# List all source files (alphabetically)
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
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/DatasetsGroup/DatasetsGroupReferenceMetadata.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/File/File.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/File/FileHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Stream/StreamContainer.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Stream/StreamHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/BoxesDefinitions/Stream/StreamProtection.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/BitStreams/InputBitstream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/BitStreams/OutputBitstream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/Signature.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignatureInputInteger.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/Signatures.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignaturesInputStream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignatureSizeComputation.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Signatures/SignaturesOutputStream.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/ByteArray.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/EncodingParameters.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/FromFile.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataStructures/Label.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataUnits/DataUnitAccessUnit.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataUnits/DataUnitBlockHeader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataUnits/DataUnits.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DataUnits/DataUnitsParametersSet.c)
set(format_source_files ${format_source_files} ${format_source_dir}/FileHandlingUtils/FileHandlingUtils.c)
set(format_source_files ${format_source_files} ${format_source_dir}/AccessUnitTree.c)
set(format_source_files ${format_source_files} ${format_source_dir}/AccessUnitTreeNode.c)
set(format_source_files ${format_source_files} ${format_source_dir}/Boxes.c)
set(format_source_files ${format_source_files} ${format_source_dir}/ClassesSet.c)
set(format_source_files ${format_source_files} ${format_source_dir}/DescriptorTreeNode.c)
set(format_source_files ${format_source_files} ${format_source_dir}/FastaReader.c)
set(format_source_files ${format_source_files} ${format_source_dir}/rbtree.c)
set(format_source_files ${format_source_files} ${format_source_dir}/treeUint64.c)
set(format_source_files ${format_source_files} ${format_source_dir}/utils.c)
set(format_source_files ${format_source_files} ${format_source_dir}/vector.c)
set(format_source_files ${format_source_files} ${format_source_dir}/VectorUint64.c)

# List all header files (alphabetically)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/BitStreams/InputBitstream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/BitStreams/OutputBitstream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/Signature.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignatureInputInteger.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/Signatures.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignaturesInputStream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignatureSizeComputation.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/Signatures/SignaturesOutputStream.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataStructures/FromFile.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DataUnits/DataUnits.h)
set(format_header_files ${format_header_files} ${format_source_dir}/FileHandlingUtils/FileHandlingUtils.h)
set(format_header_files ${format_header_files} ${format_header_dir}/AccessUnitTree.h)
set(format_header_files ${format_header_files} ${format_header_dir}/AccessUnitTreeNode.h)
set(format_header_files ${format_header_files} ${format_header_dir}/Boxes.h)
set(format_header_files ${format_header_files} ${format_header_dir}/ClassesSet.h)
set(format_header_files ${format_header_files} ${format_header_dir}/DescriptorTreeNode.h)
set(format_header_files ${format_header_files} ${format_header_dir}/EncodingParameters.h)
set(format_header_files ${format_header_files} ${format_header_dir}/FastaReader.h)
set(format_header_files ${format_header_files} ${format_header_dir}/portability.h)
set(format_header_files ${format_header_files} ${format_header_dir}/rbtree.h)
set(format_header_files ${format_header_files} ${format_header_dir}/treeUint64.h)
set(format_header_files ${format_header_files} ${format_header_dir}/utils.h)
set(format_header_files ${format_header_files} ${format_header_dir}/utilTree.h)
set(format_header_files ${format_header_files} ${format_header_dir}/vector.h)
set(format_header_files ${format_header_files} ${format_header_dir}/vectorUint64.h)

# Group the source and header files
source_group("format Source Files" FILES ${format_source_files})
source_group("format Header Files" FILES ${format_header_files})

# Set up the target
add_library(${format} STATIC ${format_source_files})
target_include_directories(${format} PRIVATE ${format_include_dir})


#==============================================================================
# MPEGGFileCreation library
#==============================================================================

# Set the paths
set(MPEGGFileCreation "MPEGGFileCreation")
set(MPEGGFileCreation_source_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation)
set(MPEGGFileCreation_header_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation)
set(MPEGGFileCreation_include_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation ${CMAKE_SOURCE_DIR}/source)

# List all source files (alphabetically)
set(MPEGGFileCreation_source_files ${MPEGGFileCreation_source_files} ${MPEGGFileCreation_source_dir}/MPEGGFileCreator.cpp)

# List all header files (alphabetically)
set(MPEGGFileCreation_header_files ${MPEGGFileCreation_header_files} ${MPEGGFileCreation_header_dir}/MPEGGFileCreator.h)

# Group the source and header files
source_group("MPEGGFileCreation Source Files" FILES ${MPEGGFileCreation_source_files})
source_group("MPEGGFileCreation Header Files" FILES ${MPEGGFileCreation_header_files})

# Set up the target
add_library(${MPEGGFileCreation} STATIC ${MPEGGFileCreation_source_files})
target_include_directories(${MPEGGFileCreation} PRIVATE ${MPEGGFileCreation_include_dir})
target_link_libraries(${MPEGGFileCreation} ${format})

#==============================================================================
# conformance
#==============================================================================

add_subdirectory(source/conformance ${CMAKE_CURRENT_BINARY_DIR}/conformance)

#==============================================================================
# genie
#==============================================================================

# Set the paths
set(genie "genie")
set(genie_source_dir ${CMAKE_SOURCE_DIR}/source )
set(genie_header_dir ${CMAKE_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/gabac-src/source)
set(genie_include_dir ${CMAKE_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/gabac-src/source)

# List all source files
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
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/gabac_integration.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/generation.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/genie_file_format.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/ProgramOptions.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/thread_pool.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/utilities.cc)

# List all header files (alphabetically)
# TBD

# Group the source and header files
source_group("genie Source Files" FILES ${genie_source_files})
# source_group("genie Header Files" FILES ${genie_header_files})

# Set up the target
add_executable(${genie} ${genie_source_files})
if (${GENIE_USE_OPENMP})
    target_compile_definitions(${genie} PRIVATE GENIE_USE_OPENMP)
endif ()
target_include_directories(${genie} PRIVATE ${genie_include_dir})

target_link_libraries(${genie} ${format})
target_link_libraries(${genie} ${file})
target_link_libraries(${genie} ${MPEGGFileCreation})
target_link_libraries(${genie} ${coding})
target_link_libraries(${genie} ${file})
target_link_libraries(${genie} gabac)


#==============================================================================
# tests
#==============================================================================

# if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    # Set the paths
    set(tests "tests")
    set(tests_source_dir ${CMAKE_SOURCE_DIR}/tests)
    set(tests_header_dir ${CMAKE_SOURCE_DIR}/tests)
    set(tests_include_dir ${CMAKE_SOURCE_DIR})

    # List all source files (alphabetically)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/main.cpp)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/example_test.cpp)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/format/EncodingParameters_tests.cpp)
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/utils/MPEGGFileCreation/MPEGGFileCreatorTest.cpp)

    # List all header files (alphabetically)
    # TBD

    # Group the source and header files
    source_group("tests Source Files" FILES ${tests_source_files})
    # source_group("tests Header Files" FILES ${tests_header_files})

    # Set up the target
    add_executable(${tests} EXCLUDE_FROM_ALL ${tests_source_files})
    target_include_directories(${tests} PRIVATE ${format_include_dir})
    target_include_directories(${tests} PRIVATE ${genie_include_dir})
    target_include_directories(${tests} PRIVATE ${CMAKE_BINARY_DIR})
    target_link_libraries(${tests} gtest_main)
    target_link_libraries(${tests} ${format})
    target_link_libraries(${tests} ${MPEGGFileCreation})
# endif ()
