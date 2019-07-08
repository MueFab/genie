project(example-tests)

set(source_files
        example-test.cc
)

add_executable(example-tests ${source_files})

target_link_libraries(example-tests PRIVATE gtest_main)
