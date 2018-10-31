#==============================================================================
# General CMake configuration
#==============================================================================

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project("genie" C CXX)

# Custom CMake cache variables
set(GENIE_USE_OPENMP OFF CACHE BOOL "Use OpenMP parallelization.")
message(STATUS "Custom CMake cache variables:")
message(STATUS "GENIE_USE_OPENMP=${GENIE_USE_OPENMP}")


#==============================================================================
# Compiler configuration
#==============================================================================

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Detected compiler: GNU Compiler Collection")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -dl")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wfloat-equal")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wwrite-strings")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Waggregate-return")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-qual")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-enum")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunreachable-code")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")

    if (${GENIE_USE_OPENMP})
        find_package( OpenMP REQUIRED)
        message(STATUS "OPENMP FOUND")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    endif ()

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "Detected compiler: LLVM Clang or Apple Clang")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Detected compiler: Microsoft Visual Studio")
    message(FATAL_ERROR "Compiler currently not supported by CMake build: ${CMAKE_CXX_COMPILER_ID}")
else ()
    message(FATAL_ERROR "Compiler currently not supported by CMake build: ${CMAKE_CXX_COMPILER_ID}")
endif ()


#==============================================================================
# Boost
#==============================================================================

find_package(Boost 1.67 COMPONENTS program_options filesystem system REQUIRED)


#==============================================================================
# loguru
#==============================================================================

add_library(loguru INTERFACE)
target_include_directories(loguru INTERFACE  ${CMAKE_SOURCE_DIR}/source/common/logging)
find_package(Threads REQUIRED)

if (Threads_FOUND)
    message(STATUS "Threads found")
else ()
    message(FATAL_ERROR "Threads not found")
endif()


#==============================================================================
# genie
#==============================================================================

set(genie "genie")

set(genie_source_dir ${CMAKE_SOURCE_DIR}/source)
set(genie_header_dir ${CMAKE_SOURCE_DIR}/source)
set(genie_include_dir ${CMAKE_SOURCE_DIR}/source)

set(logging_include_dir ${CMAKE_SOURCE_DIR}/source/utils/logging)

set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/dsg.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/exceptions.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/FastaFileReader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/FastaRecord.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/FastqFileReader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/FastqRecord.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/FileReader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/generation.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/ProgramOptions.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/SamFileReader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/SamRecord.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/utilities.cc)

set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/spring.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/preprocess.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/util.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/bitset_util.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/encoder.cpp)
# set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/pe_encode.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/reorder_compress_quality_id.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/generate_read_streams.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/generate_new_fastq.cpp)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/descriptors/spring/decode_read_streams.cpp)

add_executable(${genie} ${genie_source_files})

if (${GENIE_USE_OPENMP})
    target_compile_definitions(${genie} PRIVATE GENIE_USE_OPENMP)
endif ()

target_include_directories(${genie} PRIVATE ${genie_include_dir})
target_include_directories(${genie} PRIVATE ${logging_include_dir})
# target_include_directories(${genie} PRIVATE ${boost_include_dir})

# #target_link_libraries(${genie} ${boost_lib_dir}/libboost_log.a)
target_link_libraries(${genie} Boost::program_options)
target_link_libraries(${genie} Boost::filesystem)
target_link_libraries(${genie} Boost::system)
# target_link_libraries(${genie} ${CMAKE_DL_LIBS})
