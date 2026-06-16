project("genie-jbig")

set(source_files
        encoder.cc
        decoder.cc
        jbig_interface.c
)
add_library(genie-jbig ${source_files})

find_package(JBIG REQUIRED)

target_link_libraries(genie-jbig PRIVATE genie-core)
target_link_libraries(genie-jbig PRIVATE genie-util)
target_link_libraries(genie-jbig PRIVATE ${JBIG_LIBRARIES})

target_include_directories(genie-jbig PRIVATE ${JBIG_INCLUDE_DIRS})
