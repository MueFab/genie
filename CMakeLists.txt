cmake_minimum_required(VERSION 3.1)


project("genie")

# Download boost from git

include(ExternalProject)

if ( UNIX )
  if (NOT Boost)

  SET (Boost_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/include/boost/)
  SET (Boost_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/lib )
  SET (BOOST_URL https://github.com/boostorg/boost.git )
  SET (BOOST_BUILD ${CMAKE_CURRENT_BINARY_DIR}) 
  SET (FILESYSTEM_STATIC_LIBRARIES ${BOOST_BUILD}/lib/libboost_filesystem.a) 
  SET (SYSTEM_STATIC_LIBRARIES ${BOOST_BUILD}/lib/libboost_system.a)
  SET (REGEX_STATIC_LIBRARIES ${BOOST_BUILD}/lib/libboost_regex.a)
  SET (PROGRAM_OPTIONS_STATIC_LIBRARIES ${BOOST_BUILD}/lib/libboost_program_options.a)

  ExternalProject_Add( Boost
    PREFIX Boost
    GIT_REPOSITORY ${BOOST_URL}
    UPDATE_COMMAND ./bootstrap.sh --with-libraries=filesystem,system,regex,program_options --includedir=${CMAKE_BINARY_DIR}/include/
    CONFIGURE_COMMAND ./b2 headers
    BUILD_COMMAND ./b2 link=static install --libdir=${CMAKE_BINARY_DIR}/lib
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
  )

  ADD_LIBRARY (Boost_LIB STATIC IMPORTED DEPENDS Boost)  
  SET_TARGET_PROPERTIES (Boost_LIB PROPERTIES IMPORTED_LOCATION ${PROGRAM_OPTIONS_STATIC_LIBRARIES}) 

  endif()
endif()


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
set(dsg_source_dir ${CMAKE_SOURCE_DIR}/source/dsg)
set(dsg_include_dir ${CMAKE_SOURCE_DIR}/source/dsg)

set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/dsg.cc)
set(dsg_source_files ${dsg_source_files} ${dsg_source_dir}/common/helpers.cc)


set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/common/constants.h)
set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/common/helpers.h)
set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/common/log.h)
set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/common/os.h)
set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/common/return-codes.h)

# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/cmdline.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/config.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/environment_iterator.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/eof_iterator.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/errors.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/option.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/options_description.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/parsers.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/positional_options.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/value_semantic.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/variables_map.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/version.hpp)
# 
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/detail/cmdline.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/detail/config_file.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/detail/convert.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/detail/parsers.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/detail/utf8_codecvt_facet.hpp)
# set(dsg_header_files ${dsg_header_files} ${dsg_include_dir}/vendor/program_options/detail/value_sema.hpp)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_executable(dsg ${dsg_source_files})
target_link_libraries(${dsg} Boost_LIB)

message(STATUS ${program_optionsIncludeDir})

set(encoder "encoder")
set(encoder_source_dir ${CMAKE_SOURCE_DIR}/source/encoder)
set(encoder_include_dir ${CMAKE_SOURCE_DIR}/source/encoder)

set(encoder_source_files ${encoder_source_files} ${encoder_source_dir}/encoder.cc)

#set(encoder_header_files ${encoder_header_files} )

add_executable(encoder ${encoder_source_files})

target_include_directories(${encoder} PRIVATE ${encoder_include_dir})
