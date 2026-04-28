set(name "genie-base")

set(source_files
    encoder.cc
    encoder.h
)

add_library(${name} ${source_files})

target_link_libraries(${name} PRIVATE genie-core)
