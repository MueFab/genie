project("genie-contact")

set(source_files
        consts.h
        subcontact_matrix_parameters.cc
        subcontact_matrix_payload.cc
        subcontact_matrix_mask_payload.cc
        contact_matrix_parameters.cc
        contact_matrix_tile_payload.cc
        contact_matrix_bin_payload.cc
)

if(GENIE_CONTACT_BACKEND STREQUAL "xtensor")
    set(XTENSOR_USE_XSIMD 0)
    set(XTENSOR_USE_TBB 0)

    find_package(xtl REQUIRED)
    find_package(xtensor REQUIRED)
    list(APPEND source_files contact_coder_xtensor.cc)
elseif(GENIE_CONTACT_BACKEND STREQUAL "eigen")
    find_package(Eigen REQUIRED)
    list(APPEND source_files contact_coder_eigen.cc)
else()
    list(APPEND source_files contact_coder_std.cc)
endif()

add_library(${PROJECT_NAME} ${source_files})

if(GENIE_CONTACT_BACKEND STREQUAL "xtensor")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_CONTACT_BACKEND_XTENSOR)
    
    add_library(xtensor_no_warnings INTERFACE)
    target_link_libraries(xtensor_no_warnings INTERFACE xtensor)

    # Create an interface library for xtensor with suppressed warnings
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        target_compile_options(xtensor_no_warnings INTERFACE
            -Wno-undef
            -Wno-shadow
            -Wno-pedantic
            -Wno-unused-local-typedefs
            -Wno-sign-compare
            -Wno-unused-variable
            -Wno-unused-parameter
            -Wno-unused-but-set-variable
            -Wno-implicit-fallthrough
            -Wno-maybe-uninitialized
            -Wno-array-bounds
            -Wno-stringop-overflow
        )
    else ()
        target_compile_options(xtensor_no_warnings INTERFACE "/w")
    endif()

    target_link_libraries(${PROJECT_NAME} PUBLIC xtensor_no_warnings)

elseif(GENIE_CONTACT_BACKEND STREQUAL "eigen")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_CONTACT_BACKEND_EIGEN)
    target_link_libraries(${PROJECT_NAME} PUBLIC eigen)
endif()

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE "-pedantic")
    target_compile_options(${PROJECT_NAME} PRIVATE "-fno-common")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wshadow")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wextra")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wconversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wdouble-promotion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=conversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=sign-conversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-strict-aliasing")
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)
#target_link_libraries(${PROJECT_NAME} PUBLIC mpeggCodecs-shared)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-jbig)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()

target_include_directories(${PROJECT_NAME} PUBLIC "${TOP_DIR}")