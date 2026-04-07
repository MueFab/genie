project("genie-annotation")



set(source_files
        annotation.cc
        geno_annotation.cc
        site_annotation.cc
        sample_annotation.cc
        gene_expression_annotation.cc
        feature_annotation.cc
        contact_annotation.cc
        functional_annotation.cc
        track_data_annotation.cc
        trackproperty_annotation.cc
        compressors.cc
        parameterset_composer.cc
        annotation_encoder.cc
        json_attribute_parser.cc
)


add_library(genie-annotation ${source_files})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-core)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-bsc)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-lzma)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-zstd)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-ser)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-encoder-factory)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-functionalannotation)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-track)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-trackproperty)


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
