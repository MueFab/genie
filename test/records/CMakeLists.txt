project("records-tests")

set(source_files
        helpers.cc
        variant_genotype-test.cc
        contact-test.cc
)

add_executable(records-tests ${source_files})

target_link_libraries(records-tests PRIVATE gtest_main)
target_link_libraries(records-tests PRIVATE genie-core)
target_link_libraries(records-tests PRIVATE genie-util)

install(TARGETS records-tests
        RUNTIME DESTINATION "usr/bin")