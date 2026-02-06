project("genie-likelihood")

set(source_files
        likelihood_parameters.cc
        likelihood_payload.cc
)

if(GENIE_LIKELIHOOD_BACKEND STREQUAL "xtensor")
    set(XTENSOR_USE_XSIMD 0)
    set(XTENSOR_USE_TBB 0)

    find_package(xtl REQUIRED)
    find_package(xtensor REQUIRED)
    list(APPEND source_files likelihood_coder_xtensor.cc)
elseif(GENIE_LIKELIHOOD_BACKEND STREQUAL "eigen")
    find_package(Eigen REQUIRED)
    list(APPEND source_files likelihood_coder_eigen.cc)
else()
    list(APPEND source_files likelihood_coder_std.cc)
endif()

add_library(${PROJECT_NAME} ${source_files})

if(GENIE_LIKELIHOOD_BACKEND STREQUAL "xtensor")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_LIKELIHOOD_BACKEND_XTENSOR)
    target_link_libraries(${PROJECT_NAME} PUBLIC xtensor_no_warnings)
elseif(GENIE_LIKELIHOOD_BACKEND STREQUAL "eigen")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_LIKELIHOOD_BACKEND_EIGEN)
    target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_LIKELIHOOD_BACKEND_STD)
endif()

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
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=sign-conversion")
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
