project("genotype-tests")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

message(STATUS "xtensor: ${xtensor_INCLUDE_DIRS}")

set(source_files
        helpers.cc
        encode-test.cc
)

add_executable(genotype-tests ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(genotype-tests PRIVATE gtest_main)
#target_link_libraries(genotype-tests PRIVATE xtensor)
target_link_libraries(genotype-tests PRIVATE xtensor xtensor::optimize)
target_link_libraries(genotype-tests PRIVATE genie-core)
target_link_libraries(genotype-tests PRIVATE genie-util)
target_link_libraries(genotype-tests PRIVATE genie-gvc)

install(TARGETS genotype-tests
        RUNTIME DESTINATION "usr/bin")