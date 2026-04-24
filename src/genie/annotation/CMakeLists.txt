project("genie-annotation")

set(source_files
        annotation.cc
		geno_annotation.cc
		site_annotation.cc
		contact_annotation.cc
        compressors.cc
		parameterset_composer.cc
		annotation_encoder.cc
		json_attribute_parser.cc
		variantsite_parser.cc
		vsite_parameterset_composer.cc
		accessunit_composer.cc
		vsite_attributes.cc
		vsite_descriptors.cc
		vsite_tiles.cc
)

add_library(${PROJECT_NAME} ${source_files})

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

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-bsc)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-lzma)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-zstd)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-jbig)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-contact)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-genotype)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-likelihood)


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
