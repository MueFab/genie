project("genie-demultiplextag")

set(source_files
        encoder.cc
        decoder.cc)

add_library(genie-demultiplextag ${source_files})

target_link_libraries(genie-demultiplextag PUBLIC genie-core)