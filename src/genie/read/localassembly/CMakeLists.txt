project("genie-localassembly")

set(source_files
        decoder.cc
        encoder.cc
        local-reference.cc)

add_library(genie-localassembly ${source_files})

target_link_libraries(genie-localassembly PUBLIC genie-paramqv1)
target_link_libraries(genie-localassembly PUBLIC genie-basecoder)
target_link_libraries(genie-localassembly PUBLIC genie-core)
target_link_libraries(genie-localassembly PUBLIC genie-nametoken)