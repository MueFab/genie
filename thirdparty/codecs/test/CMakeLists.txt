cmake_minimum_required(VERSION 2.9.0)
project(Test)

set(TEST_RESOURCES_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(TEST_OUTPUT_TMP_FILE  ${PROJECT_BINARY_DIR}/output.tmp)

set(INC_LIBRARIES
        Api
        Libbsc
        Lzma
        Zstd)

set(LINK_LIBRARIES mpeggCodecs-static)
include(apply_dependencies NO_POLICY_SCOPE)

set(TEST_NAME "mpegg-libbsc")
add_executable(${TEST_NAME} ${TEST_NAME}.c)
target_link_libraries(${TEST_NAME} ${LINK_LIBRARIES})
add_test(NAME API_${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}> ${TEST_RESOURCES_PATH}/r1.fastq)


set(TEST_NAME "mpegg-lzma")
add_executable(${TEST_NAME} ${TEST_NAME}.c)
target_link_libraries(${TEST_NAME} ${LINK_LIBRARIES})
add_test(NAME API_${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}> ${TEST_RESOURCES_PATH}/r1.fastq)


set(TEST_NAME "mpegg-zstd")
add_executable(${TEST_NAME} ${TEST_NAME}.c)
target_link_libraries(${TEST_NAME} ${LINK_LIBRARIES})
add_test(NAME API_${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}> ${TEST_RESOURCES_PATH}/r1.fastq)




