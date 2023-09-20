project("genie")

set(source_files
        main.cc
        run/main.cc
        run/program-options.cc
        gabac/code.cc
        gabac/main.cc
        gabac/program-options.cc
        annotation/main.cc
        annotation/program-options.cc
        annotation/code.cc
        )

add_executable(genie ${source_files})

target_link_libraries(genie PRIVATE genie-core)
target_link_libraries(genie PRIVATE genie-util)
target_link_libraries(genie PRIVATE genie-bsc)
target_link_libraries(genie PRIVATE genie-lzma)
target_link_libraries(genie PRIVATE genie-zstd)
target_link_libraries(genie PRIVATE genie-module)
target_link_libraries(genie PRIVATE genie-gabac)
target_link_libraries(genie PRIVATE genie-vsite)
target_link_libraries(genie PRIVATE mpeggCodecs-static)

#target_link_libraries(genie PRIVATE libbsc)
#target_link_libraries(genie PRIVATE Api)
# TODO: To be implemented later
#target_link_libraries(genie PRIVATE genie-mgrec)
#target_link_libraries(genie PRIVATE genie-mgg)
#target_link_libraries(genie PRIVATE genie-mgb)

#target_link_libraries(genie PRIVATE genie-spring)
#target_link_libraries(genie PRIVATE genie-lowlatency)
#target_link_libraries(genie PRIVATE genie-fastq)
#target_link_libraries(genie PRIVATE genie-localassembly)
#target_link_libraries(genie PRIVATE genie-qvwriteout)
#target_link_libraries(genie PRIVATE genie-calq)

install(TARGETS genie
        RUNTIME DESTINATION "usr/bin")
