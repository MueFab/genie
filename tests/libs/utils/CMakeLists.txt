project("utils-tests")

set(source_files
        fasta-file-reader-test.cc
        fastq-file-reader-test.cc
        )

add_executable(utils-tests ${source_files})

target_link_libraries(utils-tests PRIVATE gtest_main)
target_link_libraries(utils-tests PRIVATE utils)
