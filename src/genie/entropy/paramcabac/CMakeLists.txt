project("genie-paramcabac")

set(source_files
        binarization.cc
        binarization_parameters.cc
        context.cc
        decoder.cc
        subsequence.cc
        support_values.cc
        state_vars.cc
        transformed-seq.cc
        transformed_parameters.cc
        )

add_library(genie-paramcabac ${source_files})

target_link_libraries(genie-paramcabac PUBLIC genie-core)
target_link_libraries(genie-paramcabac PUBLIC genie-util)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-paramcabac PUBLIC "${THIS_DIR}")
target_include_directories(genie-paramcabac PUBLIC "${TOP_DIR}")