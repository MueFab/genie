project("tileConfiguration-tests")

set(source_files
        tileConfiguration-test.cc
        main.cc
)

add_executable(tileConfiguration-tests ${source_files})

target_link_libraries(tileConfiguration-tests PRIVATE gtest_main)
target_link_libraries(tileConfiguration-tests PRIVATE genie-core)
target_link_libraries(tileConfiguration-tests PRIVATE genie-util)
