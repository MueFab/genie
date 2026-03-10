project("genie-ser")

set(source_files
    encoder.cc
)
add_library(genie-ser ${source_files})

target_link_libraries(genie-ser PRIVATE genie-core)
target_link_libraries(genie-ser PRIVATE genie-util)
target_link_libraries(genie-ser PRIVATE genie-base)
target_link_libraries(genie-ser PRIVATE mpeggCodecs-static)

