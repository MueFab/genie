project("genie-lzma")

set(source_files
    encoder.cc
)
add_library(genie-lzma ${source_files})

target_link_libraries(genie-lzma PRIVATE genie-core)
target_link_libraries(genie-lzma PRIVATE genie-util)
target_link_libraries(genie-lzma PRIVATE mpeggCodecs-static)
