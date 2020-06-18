project("genie-nametoken")

set(source_files
        encoder.cc
        tokenizer.cc
        decoder.cc
        token.cc)

add_library(genie-nametoken ${source_files})

target_link_libraries(genie-nametoken PUBLIC genie-core)