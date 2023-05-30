project("mpeggBSC-tests")

set(source_files
        mpeggBSCTest.cc
        main.cc
)

add_executable(mpeggBSC-tests ${source_files})

target_link_libraries(mpeggBSC-tests PRIVATE gtest_main)
target_link_libraries(mpeggBSC-tests PRIVATE genie-core)
target_link_libraries(mpeggBSC-tests PRIVATE genie-util)
