set(name "genie-encoder-factory")

set(source_files
    encoder_factory.cc
    encoder_factory.h
)

add_library(${name} ${source_files})

target_link_libraries(${name} PRIVATE genie-core)
target_link_libraries(${name} PRIVATE genie-base)
target_link_libraries(${name} PRIVATE genie-bsc)
target_link_libraries(${name} PRIVATE genie-lzma)
target_link_libraries(${name} PRIVATE genie-ser)
target_link_libraries(${name} PRIVATE genie-zstd)

