project("genie-spring")

set(source_files
        bitset_util.cc
        call_template_functions.cc
        decoder.cc
        spring_encoding.cc
        generate_read_streams.cc
        preprocess.cc
        reorder_compress_quality_id.cc
        encoder.cc
        encoder_source.cc
        util.cc
)

add_library(genie-spring ${source_files})

target_link_libraries(genie-spring PUBLIC genie-gabac)
target_link_libraries(genie-spring PUBLIC genie-zstd)
target_link_libraries(genie-spring PUBLIC genie-lzma)
target_link_libraries(genie-spring PUBLIC genie-bsc)

target_link_libraries(genie-spring PUBLIC genie-core)
target_link_libraries(genie-spring PRIVATE ${OpenMP_CXX_LIBRARIES})
