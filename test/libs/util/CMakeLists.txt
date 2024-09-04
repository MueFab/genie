project("util-tests")

set(source_files
        watch.cc
        bitwriter.cc
        bitroundtrip.cc
        helpers.cc
        string-helpers.cc
        thread-manager.cc
)

add_executable(util-tests ${source_files})

target_link_libraries(util-tests PRIVATE gtest_main)
target_link_libraries(util-tests PRIVATE genie-core)
target_link_libraries(util-tests PRIVATE genie-util)
#target_link_libraries(util-tests PRIVATE genie-sam)

install(TARGETS util-tests
        RUNTIME DESTINATION "usr/bin")