#==============================================================================
# Gabac Lib
#==============================================================================

#==============================================================================
# Project-wide Settings
#==============================================================================

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project("GABAC" VERSION 2.0.0)

# C and C++ code shall be compliant to C(++)11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Custom CMake cache variables
set(GABAC_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared GABAC library.")
option(BUILD_DOC "Build documentation" ON)
message(STATUS "GABAC_BUILD_SHARED_LIB=${GABAC_BUILD_SHARED_LIB}")

#==============================================================================
# Compiler
#==============================================================================

message(STATUS "CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
    message(STATUS "C++ compiler: Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -inline-level=1")
else ()
    message(FATAL_ERROR "C++ compiler currently not supported by CMake build")
endif ()

#==============================================================================
# Build type
#==============================================================================

# Set default build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not specified")
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Using fall-back build type '${CMAKE_BUILD_TYPE}'")
endif ()

# Log the compiler flags for each build type
message(STATUS "C++ compiler flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Additional C++ compiler flags for build type 'Debug': ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Additional C++ compiler flags for build type 'Release': ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Additional C++ compiler flags for build type 'RelWithDebInfo': ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "Additional C++ compiler flags for build type 'MinSizeRel': ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message(STATUS "Selected build type: ${CMAKE_BUILD_TYPE}")

#==============================================================================
# gabac
#==============================================================================

# Set the paths
set(gabac "gabac")
set(gabac_source_dir ${PROJECT_SOURCE_DIR})
message("Source dir: ${gabac_source_dir}")
set(gabac_header_dir ${PROJECT_SOURCE_DIR})
message("Header dir: ${gabac_header_dir}")
get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
set(gabac_include_dir ${PARENT_DIR})
message("Include dir: ${gabac_include_dir}")


# List all source files (alphabetically)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/analysis.cpp)
#set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/binary_arithmetic_encoder.cpp)
#set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/binary_arithmetic_decoder.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/bit_input_stream.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/bit_output_stream.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/c_interface.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/configuration.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/constants.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/context_model.cpp)
#set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/context_selector.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/context_tables.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/data_block.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/decode_cabac.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/decoding.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/data_block.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/diff_coding.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/encode_cabac.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/encoding.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/equality_coding.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/exceptions.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/lut_transform.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/match_coding.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/reader.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/rle_coding.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/run.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/stream_handler.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/streams.cpp)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/writer.cpp)

# List all header files (alphabetically)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/analysis.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/binary_arithmetic_encoder.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/binary_arithmetic_decoder.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/bit_input_stream.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/bit_output_stream.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/c_interface.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/cabac_tables.h)
#set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/configuration.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/constants.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/context_model.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/context_selector.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/context_tables.h)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/data_block.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/decode_cabac.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/decoding.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/diff_coding.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/encode_cabac.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/encoding.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/equality_coding.h)
set(gabac_source_files ${gabac_source_files} ${gabac_source_dir}/exceptions.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/gabac.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/lut_transform.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/match_coding.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/reader.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/rle_coding.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/run.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/stream_handler.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/streams.h)
set(gabac_header_files ${gabac_header_files} ${gabac_header_dir}/writer.h)

# Group the source and header files
source_group("GABAC Source Files" FILES ${gabac_source_files})
source_group("GABAC Header Files" FILES ${gabac_header_files})

if (${GABAC_BUILD_SHARED_LIB})
    message(STATUS "Configured build of GABAC as *shared* library")
    add_library(${gabac} SHARED ${gabac_source_files} ${gabac_header_files})
    set_target_properties(${gabac} PROPERTIES POSITION_INDEPENDENT_CODE 1)
else ()
    message(STATUS "Configured build of GABAC as *static* library")
    add_library(${gabac} STATIC ${gabac_source_files} ${gabac_header_files})
endif ()
target_include_directories(${gabac} PRIVATE ${gabac_include_dir})

#==============================================================================
# Doxygen
#==============================================================================

find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../../docs/doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
