project("genie-lowlatency")

set(source_files
        decoder.cc
        encoder.cc)

add_library(genie-lowlatency ${source_files})

target_link_libraries(genie-lowlatency PUBLIC genie-core)
target_link_libraries(genie-lowlatency PUBLIC genie-nametoken)