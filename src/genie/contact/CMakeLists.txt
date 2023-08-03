project("cmc")

set(source_files
        contact_coder.cc
)

add_library(genie-cmc ${source_files})

target_link_libraries(genie-cmc PUBLIC genie-core)