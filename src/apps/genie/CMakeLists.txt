project("genie")

set(source_files
        main.cc
        run/main.cc
        run/program-options.cc
        transcode-fastq/main.cc
        transcode-fastq/program-options.cc
        )

add_executable(genie ${source_files})

target_link_libraries(genie PRIVATE genie-gabac)
target_link_libraries(genie PRIVATE genie-module)
target_link_libraries(genie PRIVATE genie-spring)
target_link_libraries(genie PRIVATE genie-lowlatency)
target_link_libraries(genie PRIVATE genie-fastq)
target_link_libraries(genie PRIVATE genie-mgrec)
target_link_libraries(genie PRIVATE genie-mgb)
target_link_libraries(genie PRIVATE genie-localassembly)
target_link_libraries(genie PRIVATE genie-qvwriteout)
target_link_libraries(genie PRIVATE genie-util)

if (${GENIE_USE_OPENMP})
    target_compile_definitions(genie PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(genie PRIVATE ${OpenMP_CXX_LIBRARIES})
endif ()

install(TARGETS genie
        RUNTIME DESTINATION "usr/bin")
