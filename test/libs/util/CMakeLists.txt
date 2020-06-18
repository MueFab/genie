project("util-tests")

set(source_files
        fasta-file-reader-test.cc
        sam-file-reader-test.cc
        fastq-file-reader-test.cc
        bit-output-stream-test.cc
        helpers.cc
)

add_executable(util-tests ${source_files})

target_link_libraries(util-tests PRIVATE gtest_main)
target_link_libraries(util-tests PRIVATE util)
target_link_libraries(util-tests PRIVATE genie-core)
target_link_libraries(util-tests PRIVATE genie-util)