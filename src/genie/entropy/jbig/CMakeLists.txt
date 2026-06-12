project("genie-jbig")

set(source_files
        encoder.cc
        decoder.cc
        jbig_interface.c
)
add_library(genie-jbig ${source_files})

target_link_libraries(genie-jbig PRIVATE genie-core)
target_link_libraries(genie-jbig PRIVATE genie-util)
target_link_libraries(genie-jbig PRIVATE jbigkit)
