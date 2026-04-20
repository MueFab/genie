project("genie-test-contact")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

set(source_files
        helpers.cc
        structure_test.cc
        encode_test.cc
)

add_executable(${PROJECT_NAME} ${source_files})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/test/util
)

if(GENIE_CONTACT_BACKEND STREQUAL "xtensor")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_CONTACT_BACKEND_XTENSOR)
elseif(GENIE_CONTACT_BACKEND STREQUAL "eigen")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_CONTACT_BACKEND_EIGEN)
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
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=sign-conversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=maybe-uninitialized")
endif ()

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-contact-local)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-jbig)


install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "usr/bin")