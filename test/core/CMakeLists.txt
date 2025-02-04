project("genie-test-core")

set(source_files
        ndarray-test.cc
)

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE xtensor xtensor::optimize)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "usr/bin")
