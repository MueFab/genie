project("genie-test-genotype")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
#find_package(xtensor-io REQUIRED)

message(STATUS "xtensor: ${xtensor_INCLUDE_DIRS}")
#message(STATUS "xio: ${xio_INCLUDE_DIRS}")

set(source_files
        helpers.cc
        encode-test.cc
)

add_executable(${PROJECT_NAME} ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # GNU Compiler Collection
    target_compile_options(${PROJECT_NAME} PRIVATE "-pedantic")
    target_compile_options(${PROJECT_NAME} PRIVATE "-fno-common")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wshadow")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wextra")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wundef")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wconversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wdouble-promotion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=conversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=maybe-uninitialized")
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE xtensor xtensor::optimize)
#target_link_libraries(${PROJECT_NAME} PRIVATE xtensor-io)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-genotype)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-vsite)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-jbig)


install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "usr/bin")