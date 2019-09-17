project("gabac-tests")

set(source_files
        binary-arithmetic-decoder-test.cc
        bit-input-stream-test.cc
        bit-output-stream-test.cc
        main.cc
)

add_executable(gabac-tests ${source_files})

target_link_libraries(gabac-tests PRIVATE gtest_main)
target_link_libraries(gabac-tests PRIVATE gabac)
