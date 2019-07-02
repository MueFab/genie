#==============================================================================
# Generic
#==============================================================================

# we require at least CMake 3.1
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# Initializing our build options
option(BUILD_GABAC "Builds the Gabac library" ON)
option(BUILD_GABACIFY "Builds the Gabac library and application" ON)
option(BUILD_TESTS "Builds the googletests for gabac" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(BUILD_GABAC OR BUILD_GABACIFY)
    add_subdirectory(source/gabac)
endif()

if(BUILD_GABACIFY)
    add_subdirectory(source/gabacify)
endif()

if(BUILD_TESTS AND BUILD_GABAC)
    add_subdirectory(tests/)
endif()
