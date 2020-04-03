project("lowlatency-encoder")

set(source_files
        encoding.cc
        main.cc
        program-options.cc
        )

add_executable(lowlatency-encoder ${source_files})

target_link_libraries(lowlatency-encoder PUBLIC genie-fastq)
target_link_libraries(lowlatency-encoder PUBLIC genie-spring)
target_link_libraries(lowlatency-encoder PUBLIC genie-nametoken)
target_link_libraries(lowlatency-encoder PUBLIC genie-lowlatency)
target_link_libraries(lowlatency-encoder PUBLIC genie-qvwriteout)
target_link_libraries(lowlatency-encoder PUBLIC genie-module)
target_link_libraries(lowlatency-encoder PUBLIC genie-gabac)
target_link_libraries(lowlatency-encoder PUBLIC genie-mgb)
