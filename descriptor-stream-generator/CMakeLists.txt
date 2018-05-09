cmake_minimum_required(VERSION 3.1)


project("dsg")


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


set(dsg "dsg")
set(dsg_source_dir ${CMAKE_SOURCE_DIR}/source)
set(dsg_include_dir ${CMAKE_SOURCE_DIR}/source)

file(GLOB_RECURSE dsg_c_files ${dsg_source_dir}/*.c)
file(GLOB_RECURSE dsg_cc_files ${dsg_source_dir}/*.cc)
file(GLOB_RECURSE dsg_cpp_files ${dsg_source_dir}/*.cpp)
file(GLOB_RECURSE dsg_h_files ${dsg_source_dir}/*.h)
file(GLOB_RECURSE dsg_hpp_files ${dsg_source_dir}/*.hpp)

set(dsg_source_files ${dsg_c_files} ${dsg_cc_files} ${dsg_cpp_files})
set(dsg_header_files ${dsg_h_files} ${dsg_hpp_files})

add_executable(dsg ${dsg_source_files})

target_include_directories(${dsg} PRIVATE ${dsg_include_dir})
