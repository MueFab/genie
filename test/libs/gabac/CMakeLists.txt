project("gabac-tests")

set(source_files
        bit-input-stream-test.cc
        common.cc
        core-test.cc
        diff-coding-test.cc
        equality-coding-test.cc
        lut-transform-test.cc
        main.cc
        match-coding-test.cc
        rle-coding-test.cc
)

add_executable(gabac-tests ${source_files})

target_link_libraries(gabac-tests PRIVATE gtest_main)
target_link_libraries(gabac-tests PRIVATE genie-core)
target_link_libraries(gabac-tests PRIVATE genie-gabac)