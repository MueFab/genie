project("cmc")

set(source_files
        contact_coder.cc
        contact_parameters.cc
)

add_library(genie-cmc ${source_files})

target_link_libraries(genie-cmc PUBLIC genie-core)