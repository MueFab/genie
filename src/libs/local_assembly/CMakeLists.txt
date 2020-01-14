project("local_assembly")

set(source_files
        local-assembly-decoder.cc
        local-assembly-encoder.cc
        reference-encoder.cc)

add_library(local_assembly ${source_files})

target_link_libraries(local_assembly PUBLIC mpegg_read)
target_link_libraries(local_assembly PUBLIC backbone)