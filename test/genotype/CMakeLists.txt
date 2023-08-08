project("genotype-tests")

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

set(XTENSOR_USE_TBB 1)

set(source_files
        encode-test.cc
        helpers.cc
)

add_executable(genotype-tests ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(genotype-tests PRIVATE gtest_main)
#target_link_libraries(genie-gvc PUBLIC xtensor)
#target_include_directories(genie-gvc PUBLIC ${XTENSOR_INCLUDE_DIRS})
target_link_libraries(genotype-tests PRIVATE xtensor xtensor::optimize)
target_link_libraries(genotype-tests PRIVATE genie-core)
target_link_libraries(genotype-tests PRIVATE genie-util)
target_link_libraries(genotype-tests PRIVATE genie-gvc)

install(TARGETS genotype-tests
        RUNTIME DESTINATION "usr/bin")