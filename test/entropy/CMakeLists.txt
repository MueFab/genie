project("genie-test-entropy")

file(GLOB source_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main genie-core genie-util genie-bsc genie-lzma genie-zstd genie-rans genie-jbig)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
