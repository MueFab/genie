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

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # GNU Compiler Collection
    #
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common")
    target_compile_options(genotype-tests PRIVATE "-Wshadow")
    target_compile_options(genotype-tests PRIVATE "-pedantic")
    target_compile_options(genotype-tests PRIVATE "-Wall")
    target_compile_options(genotype-tests PRIVATE "-Wextra")
    target_compile_options(genotype-tests PRIVATE "-Wshadow")
    target_compile_options(genotype-tests PRIVATE "-Wundef")
    target_compile_options(genotype-tests PRIVATE "-Wconversion")
    target_compile_options(genotype-tests PRIVATE "-Wdouble-promotion")
    target_compile_options(genotype-tests PRIVATE "-fno-common")
endif ()

target_link_libraries(genotype-tests PRIVATE gtest_main)
#target_link_libraries(genotype-tests PRIVATE xtensor)
target_link_libraries(genotype-tests PRIVATE xtensor xtensor::optimize)
target_link_libraries(genotype-tests PRIVATE genie-core)
target_link_libraries(genotype-tests PRIVATE genie-util)
target_link_libraries(genotype-tests PRIVATE genie-gvc)

install(TARGETS genotype-tests
        RUNTIME DESTINATION "usr/bin")