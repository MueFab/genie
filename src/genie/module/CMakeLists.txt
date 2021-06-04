project("genie-module")

set(source_files
        default-setup.cc
        manager.cc
        )

add_library(genie-module ${source_files})


target_link_libraries(genie-module PUBLIC genie-util)
target_link_libraries(genie-module PUBLIC genie-core)
target_link_libraries(genie-module PUBLIC genie-refcoder)
target_link_libraries(genie-module PUBLIC genie-fastq)
target_link_libraries(genie-module PUBLIC genie-mgrec)
target_link_libraries(genie-module PUBLIC genie-nametoken)
target_link_libraries(genie-module PUBLIC genie-fasta)
target_link_libraries(genie-module PUBLIC genie-mgb)
target_link_libraries(genie-module PUBLIC genie-qvwriteout)
target_link_libraries(genie-module PUBLIC genie-localassembly)
target_link_libraries(genie-module PUBLIC genie-lowlatency)
#target_link_libraries(genie-module PUBLIC genie-mpegg_p1)
target_link_libraries(genie-module PUBLIC genie-gabac)
target_link_libraries(genie-module PUBLIC genie-spring)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-module PUBLIC "${THIS_DIR}")
target_include_directories(genie-module PUBLIC "${TOP_DIR}")