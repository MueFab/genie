project("util-tests")

set(source_files
        date.cc
        watch.cc
        bitwriter.cc
        bitroundtrip.cc
        helpers.cc
        sam-file-reader-test.cc
        stringview.cc
        string-helpers.cc
        thread-manager.cc
)

add_executable(util-tests ${source_files})

target_link_libraries(util-tests PRIVATE gtest_main)
target_link_libraries(util-tests PRIVATE util)
target_link_libraries(util-tests PRIVATE genie-core)
target_link_libraries(util-tests PRIVATE genie-util)
target_link_libraries(util-tests PRIVATE genie-sam)
