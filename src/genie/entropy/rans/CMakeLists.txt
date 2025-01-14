project("genie-rans")

set(source_files
        encoder.cc
        decoder.cc
        commons.cc
)

add_library(genie-rans ${source_files})

target_link_libraries(genie-rans PUBLIC genie-core)
target_link_libraries(genie-rans PUBLIC genie-util)

#target_compile_options(genie-rans PRIVATE -Wno-pedantic -Wno-sign-compare -Wno-unused-function)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-rans PUBLIC "${THIS_DIR}")
target_include_directories(genie-rans PUBLIC "${TOP_DIR}")