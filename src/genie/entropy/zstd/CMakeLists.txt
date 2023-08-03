project("genie-zstd")

set(source_files
    encoder.cc
)
add_library(genie-zstd ${source_files})

target_link_libraries(genie-zstd PRIVATE genie-core)
target_link_libraries(genie-zstd PRIVATE genie-util)
target_link_libraries(genie-zstd PRIVATE mpeggCodecs-static)
