project("genie-contact")

set(source_files
    contact_coder.cc
    contact_matrix_bin_payload.cc
    contact_matrix_parameters.cc
    contact_matrix_tile_payload.cc
    subcontact_matrix_mask_payload.cc
    subcontact_matrix_parameters.cc
    subcontact_matrix_payload.cc
)

set(absolute_source_files "")
foreach(f IN LISTS source_files)
    if(IS_ABSOLUTE "${f}")
        list(APPEND absolute_source_files "${f}")
    else()
        list(APPEND absolute_source_files "${CMAKE_CURRENT_LIST_DIR}/${f}")
    endif()
endforeach()

add_library(${PROJECT_NAME} ${absolute_source_files})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
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
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC genie-core genie-util genie-backend)

get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(${PROJECT_NAME} PUBLIC "${TOP_DIR}")
