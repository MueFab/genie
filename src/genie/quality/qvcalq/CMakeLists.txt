project("genie-qvcalq")

set(source_files
        encoder.cc
        decoder.cc
        )

add_library(genie-qvcalq ${source_files})


target_link_libraries(genie-qvcalq PUBLIC genie-core)
target_link_libraries(genie-qvcalq PUBLIC genie-paramqv1)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-qvcalq PUBLIC "${THIS_DIR}")
target_include_directories(genie-qvcalq PUBLIC "${TOP_DIR}")