project("genie-mpegg_p1")

set(source_files
        access_unit_p1.cc
        dataset.cc
        dataset_header.cc
        dataset_parameter_set.cc)

add_library(genie-mpegg_p1 ${source_files})

target_link_libraries(genie-mpegg_p1 PUBLIC genie-mgb)
