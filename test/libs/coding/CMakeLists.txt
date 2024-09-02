project("coding-tests")

set(source_files
        gabac-compressor-test.cc
        helpers.cc
)

add_executable(coding-tests ${source_files})

target_link_libraries(coding-tests PRIVATE gtest_main)
target_link_libraries(coding-tests PRIVATE genie-core)
target_link_libraries(coding-tests PRIVATE genie-gabac)
target_link_libraries(coding-tests PRIVATE genie-zstd)
target_link_libraries(coding-tests PRIVATE genie-lzma)
target_link_libraries(coding-tests PRIVATE genie-bsc)

install(TARGETS coding-tests
        RUNTIME DESTINATION "usr/bin")