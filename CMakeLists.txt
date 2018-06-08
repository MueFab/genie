#==============================================================================
# General CMake configuration
#==============================================================================

cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

project("genie")


#==============================================================================
# Compiler configuration
#==============================================================================

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Detected compiler: GNU Compiler Collection")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wundef")
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

set(boost_root_dir ${CMAKE_CURRENT_BINARY_DIR}/boost)
set(boost_include_dir ${boost_root_dir}/include)
set(boost_lib_dir ${boost_root_dir}/lib)

if (UNIX)
    include(ExternalProject)

    set(BOOST_ROOT ${boost_root_dir})
    find_package(Boost)

    if (NOT Boost_FOUND)
        set(boost_git_repository https://github.com/boostorg/boost.git)
        set(boost_git_tag "boost-1.67.0")

        ExternalProject_Add(Boost
            PREFIX ${boost_root_dir}
            GIT_REPOSITORY ${boost_git_repository}
            GIT_TAG ${boost_git_tag}
            CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=program_options --prefix=${boost_root_dir}
            BUILD_COMMAND ./b2 link=static install
            BUILD_IN_SOURCE 1
            INSTALL_COMMAND ""
        )

        add_library(boost STATIC IMPORTED DEPENDS Boost)
    else ()
        message(STATUS "Boost already installed - rebuild not required")
    endif()
else ()
    message(FATAL_ERROR "Boost build currently not supported on non-UNIX-like systems")
endif()



#==============================================================================
# dsg
#==============================================================================

set(dsg "dsg")

set(dsg_source_dir ${CMAKE_SOURCE_DIR}/source/dsg)
set(dsg_include_dir ${CMAKE_SOURCE_DIR}/source/dsg)

set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/dsg.cc)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/ProgramOptions.cc)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/common/utils.cc)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/input/FileReader.cc)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/input/fasta/FastaFileReader.cc)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/input/fasta/FastaRecord.cc)

add_executable(${dsg} ${dsg_source_files})

target_include_directories(${dsg} PRIVATE ${dsg_include_dir})
target_include_directories(${dsg} PRIVATE ${boost_include_dir})

target_link_libraries(${dsg} ${boost_lib_dir}/libboost_program_options.a)


#==============================================================================
# encoder
#==============================================================================

set(encoder "encoder")

set(encoder_source_dir ${CMAKE_SOURCE_DIR}/source/encoder)
set(encoder_include_dir ${CMAKE_SOURCE_DIR}/source/encoder)

set(encoder_source_files ${encoder_source_files} ${encoder_source_dir}/encoder.cc)

add_executable(encoder ${encoder_source_files})

target_include_directories(${encoder} PRIVATE ${encoder_include_dir})
