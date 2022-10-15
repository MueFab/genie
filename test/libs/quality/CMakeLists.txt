project("quality-tests")

set(source_files
       calq-test.cpp
)

add_executable(quality-tests ${source_files})

target_link_libraries(quality-tests PRIVATE gtest_main)
target_link_libraries(quality-tests PRIVATE genie-core)
target_link_libraries(quality-tests PRIVATE genie-calq)

install(TARGETS quality-tests
        RUNTIME DESTINATION "usr/bin")