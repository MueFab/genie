project("genie-mpegg_p1")

set(source_files
        access_unit_p1.cc
        dataset.cc
        dataset_group.cc
        dataset_group_header.cc
        dataset_header.cc
        dataset_parameter_set.cc
        file_header.cc
        mpegg_file.cc)

add_library(genie-mpegg_p1 ${source_files})

target_link_libraries(genie-mpegg_p1 PUBLIC genie-mgb)
