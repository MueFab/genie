project("local-assembly-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
        )

add_executable(local-assembly-encoder ${source_files})

target_link_libraries(local-assembly-encoder PRIVATE genie-gabac)
target_link_libraries(local-assembly-encoder PRIVATE genie-mgb)
target_link_libraries(local-assembly-encoder PRIVATE genie-sam)
target_link_libraries(local-assembly-encoder PRIVATE genie-localassembly)
target_link_libraries(local-assembly-encoder PRIVATE genie-module)
target_link_libraries(local-assembly-encoder PRIVATE genie-qvwriteout)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(local-assembly-encoder PUBLIC "${THIS_DIR}")
target_include_directories(local-assembly-encoder PUBLIC "${TOP_DIR}")