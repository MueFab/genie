project("glc")

set(source_files
        likelihood_coder.cc
        likelihood_parameters.h
)

add_library(genie-glc ${source_files})

target_link_libraries(genie-glc PUBLIC genie-core)