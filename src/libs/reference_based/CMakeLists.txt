project("reference_based")

set(source_files
        reference-based-encoder.cc)

add_library(reference_based ${source_files})

target_link_libraries(reference_based PUBLIC backbone)