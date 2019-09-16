# Download and unpack Google Test at configure time
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/CMakeListsGoogleTest.txt.in
    ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
)
if(result)
    message(FATAL_ERROR "CMake step for Google Test failed: ${result}")
endif()
execute_process(
    COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
)
if(result)
    message(FATAL_ERROR "Build step for Google Test failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker settings on
# Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add Google Test directly to our build and define the gtest and gtest_main
# targets
add_subdirectory(
    ${CMAKE_BINARY_DIR}/googletest-source
    ${CMAKE_BINARY_DIR}/googletest-build
    EXCLUDE_FROM_ALL
)
