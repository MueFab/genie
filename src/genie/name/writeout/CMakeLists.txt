project("genie-namewriteout")

set(source_files
        encoder.cc
        decoder.cc)

add_library(genie-namewriteout ${source_files})

target_link_libraries(genie-namewriteout PUBLIC genie-core)