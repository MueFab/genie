project("genie-test-annotation")

file(GLOB source_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main genie-core genie-util genie-annotation genie-genotype-local)

add_subdirectory(parameter_set)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
