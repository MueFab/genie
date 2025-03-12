project("read-tests")

set(source_files
        local-reference-test.cpp
        spring-util-test.cc
)

add_executable(read-tests ${source_files})

target_link_libraries(read-tests PRIVATE gtest_main)
target_link_libraries(read-tests PRIVATE genie-core)
target_link_libraries(read-tests PRIVATE genie-localassembly)

install(TARGETS read-tests
        RUNTIME DESTINATION "usr/bin")