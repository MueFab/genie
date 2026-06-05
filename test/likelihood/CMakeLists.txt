project("genie-test-likelihood")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

if(GENIE_LIKELIHOOD_BACKEND STREQUAL "xtensor")
    find_package(xtl REQUIRED)
    find_package(xtensor REQUIRED)
endif()
set(source_files
        helpers.cc
        encode_test.cc
        structure_test.cc
)

add_executable(${PROJECT_NAME} ${source_files})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/test/util
)

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
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=sign-conversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=maybe-uninitialized")
endif ()

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
if(GENIE_LIKELIHOOD_BACKEND STREQUAL "xtensor")
    target_link_libraries(${PROJECT_NAME} PUBLIC xtensor)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-likelihood)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-lzma)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "usr/bin")