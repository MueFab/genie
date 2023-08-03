project("gvc")

set(source_files
        genotype_coder.cc
        genotype_parameters.cc
)

add_library(genie-gvc ${source_files})

target_link_libraries(genie-gvc PUBLIC genie-core)