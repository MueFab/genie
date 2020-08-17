project("genie-refcoder")

set(source_files
        decoder.cc
        encoder.cc)

add_library(genie-refcoder ${source_files})

target_link_libraries(genie-refcoder PUBLIC genie-core)