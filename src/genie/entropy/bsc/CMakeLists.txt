project("genie-bsc")

set(source_files
    encoder.cc
        ../paramcabac/decoder.h
)
add_library(genie-bsc ${source_files})

target_link_libraries(genie-bsc PRIVATE genie-core)
target_link_libraries(genie-bsc PRIVATE genie-util)
target_link_libraries(genie-bsc PRIVATE mpeggCodecs-static)
