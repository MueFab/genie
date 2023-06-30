project("genie-bsc")

set(source_files
    encoder.cc
    
)
add_library(genie-bsc ${source_files})

target_link_libraries(genie INTERFACE mpeggCodecs-static)
target_link_libraries(genie-bsc PUBLIC genie-core)