project("genie-module")

set(source_files
        manager.cc
        )

add_library(genie-module ${source_files})


target_link_libraries(genie-module PUBLIC genie-core)
target_link_libraries(genie-module PUBLIC genie-paramqv1)
target_link_libraries(genie-module PUBLIC genie-paramcabac)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-module PUBLIC "${THIS_DIR}")
target_include_directories(genie-module PUBLIC "${TOP_DIR}")