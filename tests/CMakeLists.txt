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
    set(tests_source_files ${tests_source_files} ${tests_source_dir}/format/DataUnitsTest.cpp)
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
