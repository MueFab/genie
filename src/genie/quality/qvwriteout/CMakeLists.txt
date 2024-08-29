project("genie-qvwriteout")

set(source_files
        encoder.cc
        encoder-none.cc
)

add_library(genie-qvwriteout ${source_files})


target_link_libraries(genie-qvwriteout PUBLIC genie-core)
target_link_libraries(genie-qvwriteout PUBLIC genie-paramqv1)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-qvwriteout PUBLIC "${THIS_DIR}")
target_include_directories(genie-qvwriteout PUBLIC "${TOP_DIR}")