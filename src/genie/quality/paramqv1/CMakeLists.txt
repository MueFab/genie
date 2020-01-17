project("genie-paramqv1")

set(source_files
        codebook.cc
        parameter_set.cc
        qv_coding_config_1.cc
        )

add_library(genie-paramqv1 ${source_files})

target_link_libraries(genie-paramqv1 PUBLIC genie-core)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-paramqv1 PUBLIC "${THIS_DIR}")
target_include_directories(genie-paramqv1 PUBLIC "${TOP_DIR}")