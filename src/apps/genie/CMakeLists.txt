project("genie")

set(source_files
        main.cc
        run/main.cc
        run/program_options.cc
        transcode-fasta/main.cc
        transcode-fasta/program_options.cc
        transcode-fastq/main.cc
        transcode-fastq/program_options.cc
        gabac/code.cc
        gabac/main.cc
        gabac/program_options.cc
        capsulator/program_options.cc
        capsulator/main.cc
)

if (${GENIE_SAM_SUPPORT})
    set(source_files ${source_files}
            transcode-sam/main.cc
            transcode-sam/program_options.h
            transcode-sam/program_options.cc
    )
endif ()

add_executable(genie ${source_files})

target_link_libraries(genie PRIVATE genie-mgg)
target_link_libraries(genie PRIVATE genie-gabac)
target_link_libraries(genie PRIVATE genie-zstd)
target_link_libraries(genie PRIVATE genie-lzma)
target_link_libraries(genie PRIVATE genie-bsc)
target_link_libraries(genie PRIVATE genie-module)
target_link_libraries(genie PRIVATE genie-spring)
target_link_libraries(genie PRIVATE genie-lowlatency)
target_link_libraries(genie PRIVATE genie-fastq)
target_link_libraries(genie PRIVATE genie-sam)
target_link_libraries(genie PRIVATE genie-mgrec)
target_link_libraries(genie PRIVATE genie-mgb)
target_link_libraries(genie PRIVATE genie-localassembly)
target_link_libraries(genie PRIVATE genie-qvwriteout)
target_link_libraries(genie PRIVATE genie-calq)
target_link_libraries(genie PRIVATE genie-util)

if (${GENIE_USE_OPENMP})
    target_compile_definitions(genie PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(genie PRIVATE ${OpenMP_CXX_LIBRARIES})
endif ()

if (${GENIE_SAM_SUPPORT})
    target_compile_definitions(genie PRIVATE GENIE_SAM_SUPPORT)
    target_link_libraries(genie ${LIBLZMA_LIBRARIES})
    target_link_libraries(genie ${BZIP2_LIBRARIES})
    target_link_libraries(genie ${ZLIB_LIBRARIES})

    find_package(HTSlib REQUIRED)  # NOLINT
    target_include_directories(genie PRIVATE ${HTSlib_INCLUDE_DIRS})
    target_link_libraries(genie PRIVATE ${HTSlib_LIBRARIES})
endif ()


set_target_properties(genie PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install(TARGETS genie
        RUNTIME DESTINATION "/usr/local/bin")
