project("genie-test-records")

set(source_files
        helpers.cc
        variant_genotype-test.cc
        contact-test.cc
)

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "usr/bin")