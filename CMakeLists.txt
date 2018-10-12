#==============================================================================
# General CMake configuration
#==============================================================================

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project("genie" C CXX)


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
    FIND_PACKAGE( OpenMP REQUIRED)
    message(STATUS "OPENMP FOUND")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "Detected compiler: LLVM Clang or Apple Clang")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
    FIND_PACKAGE( OpenMP REQUIRED)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Detected compiler: Microsoft Visual Studio")
    message(FATAL_ERROR "Compiler currently not supported by CMake build: ${CMAKE_CXX_COMPILER_ID}")
else ()
    message(FATAL_ERROR "Compiler currently not supported by CMake build: ${CMAKE_CXX_COMPILER_ID}")
endif ()


#==============================================================================
# Boost
#==============================================================================

set(boost_root_dir ${CMAKE_CURRENT_BINARY_DIR}/boost)
set(boost_include_dir ${boost_root_dir}/include)
set(boost_lib_dir ${boost_root_dir}/lib)

if (UNIX)
    include(ExternalProject)

    set(BOOST_ROOT ${boost_root_dir})
    set(Boost_NO_SYSTEM_PATHS "ON")
    if (NOT Boost_FOUND)

        set(boost_git_repository https://github.com/boostorg/boost.git)
        set(boost_git_tag "boost-1.67.0")

        ExternalProject_Add(Boost
            PREFIX ${boost_root_dir}
            GIT_REPOSITORY ${boost_git_repository}
            GIT_TAG ${boost_git_tag}
            CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=log,program_options,filesystem,system --prefix=${boost_root_dir}
            BUILD_COMMAND ./b2 link=static install
            BUILD_IN_SOURCE 1
            INSTALL_COMMAND ""
        )

        add_library(boost STATIC IMPORTED DEPENDS Boost)
	else()
	        message(STATUS "Boost already installed - rebuild not required")
	endif()
else ()
    message(FATAL_ERROR "Boost build currently not supported on non UNIX-like systems")
endif()

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
# dsg
#==============================================================================

set(dsg "dsg")

set(dsg_include_dir ${CMAKE_SOURCE_DIR}/source/lib/dsg)
set(dsg_source_dir ${CMAKE_SOURCE_DIR}/source/lib/dsg)
set(app_include_dir ${CMAKE_SOURCE_DIR}/source/app)
set(app_source_dir ${CMAKE_SOURCE_DIR}/source/app)
set(logging_include_dir ${CMAKE_SOURCE_DIR}/source/utils/logging)

set(dsg_header_files ${dsg_header_files} ${app_include_dir}/generation.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/ProgramOptions.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/common/utilities.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/common/exceptions.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/input/FileReader.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/input/fasta/FastaFileReader.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/input/fasta/FastaRecord.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/input/fastq/FastqFileReader.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/input/fastq/FastqRecord.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/input/sam/SamFileReader.h)
set(dsg_header_files ${dsg_header_files} ${app_include_dir}/input/sam/SamRecord.h)
set(dsg_header_files ${dsg_header_files} ${logging_include_dir}/loguru.h)

set(dsg_source_files ${dsg_source_files} ${app_source_dir}/dsg.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/generation.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/ProgramOptions.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/common/utilities.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/common/exceptions.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/FileReader.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/fasta/FastaFileReader.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/fasta/FastaRecord.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/fastq/FastqFileReader.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/fastq/FastqRecord.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/sam/SamFileReader.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/sam/SamRecord.cc)
set(dsg_source_files ${dsg_source_files} ${app_source_dir}/input/sam/SamFileReader.cc)

set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/spring.cpp)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/preprocess.cpp)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/util.cpp)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/bitset_util.cpp)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/encoder.cpp)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/pe_encode.cpp)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/reorder_compress_quality_id.cpp)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/algorithms/SPRING/generate_read_streams.cpp)

add_executable(${dsg} ${dsg_header_files} ${dsg_source_files})

target_include_directories(${dsg} PRIVATE ${dsg_include_dir})
target_include_directories(${dsg} PRIVATE ${app_include_dir})
target_include_directories(${dsg} PRIVATE ${logging_include_dir})
target_include_directories(${dsg} PRIVATE ${boost_include_dir})

# #target_link_libraries(${dsg} ${boost_lib_dir}/libboost_log.a)
target_link_libraries(${dsg} ${boost_lib_dir}/libboost_program_options.a)
target_link_libraries(${dsg} ${boost_lib_dir}/libboost_filesystem.a)
target_link_libraries(${dsg} ${boost_lib_dir}/libboost_system.a)
target_link_libraries(${dsg} ${CMAKE_DL_LIBS})


#==============================================================================
# encoder
#==============================================================================

set(encoder "encoder")

set(encoder_include_dir ${CMAKE_SOURCE_DIR}/source/lib/encoder)
set(encoder_source_dir ${CMAKE_SOURCE_DIR}/source/lib/encoder)

#set(encoder_header_files ${enocder_header_files} ${encoder_include_dir}/dummy.h)

set(encoder_source_files ${encoder_source_files} ${encoder_source_dir}/encoder.cc)

add_executable(${encoder} ${encoder_header_files} ${encoder_source_files})

target_include_directories(${encoder} PRIVATE ${encoder_include_dir})
target_include_directories(${encoder} PRIVATE ${boost_include_dir})

target_link_libraries(${encoder} ${boost_lib_dir}/libboost_program_options.a)

