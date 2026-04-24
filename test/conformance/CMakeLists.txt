project("genie-test-conformance")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

file(GLOB source_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main xtensor xtensor::optimize genie-core genie-annotation genie-util genie-genotype genie-jbig)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "usr/bin")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
