project("genie-genotype")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

set(source_files
        amax_payload.cc
        bin_mat_payload.cc
        genotype_parameters.cc
        genotype_payload.cc
        parameterset_composer.cc
        row_col_ids_payload.cc
        sorted_bin_mat_payload.cc
)

if(GENIE_GENOTYPE_BACKEND STREQUAL "xtensor")
    list(APPEND source_files genotype_coder_xtensor.cc)
elseif(GENIE_GENOTYPE_BACKEND STREQUAL "eigen")
    find_package(Eigen REQUIRED)
    list(APPEND source_files genotype_coder_eigen.cc)
else()
    list(APPEND source_files genotype_coder_std.cc)
endif()

add_library(${PROJECT_NAME} ${source_files})

if(GENIE_GENOTYPE_BACKEND STREQUAL "xtensor")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_GENOTYPE_BACKEND_XTENSOR)
elseif(GENIE_GENOTYPE_BACKEND STREQUAL "eigen")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_GENOTYPE_BACKEND_EIGEN)
    target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_GENOTYPE_BACKEND_STD)
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
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=maybe-uninitialized")
endif ()

#target_link_libraries(${PROJECT_NAME} PUBLIC xtensor xtensor::optimize)
target_link_libraries(${PROJECT_NAME} PUBLIC xtensor_no_warnings)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
#target_link_libraries(${PROJECT_NAME} PUBLIC mpeggCodecs-shared)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-jbig)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()