project("genie")

set(source_files
        main.cc
        run/main.cc
        run/program-options.cc
#        transcode-fasta/main.cc
#        transcode-fasta/program-options.cc
#        transcode-fastq/main.cc
#        transcode-fastq/program-options.cc
        gabac/code.cc
        gabac/main.cc
        gabac/program-options.cc
#        capsulator/program-options.cc
#        capsulator/main.cc
        )

#if (${GENIE_SAM_SUPPORT})
#    set(source_files ${source_files}
#            transcode-sam/main.cc
#            transcode-sam/transcoding.h
#            transcode-sam/utils.cc
#            transcode-sam/utils.h
#            transcode-sam/sam/sam_to_mgrec/program-options.cc
#            transcode-sam/sam/sam_to_mgrec/sam_record.h
#            transcode-sam/sam/sam_to_mgrec/sam_record.cc
#            transcode-sam/sam/sam_to_mgrec/transcoder.cc
#            transcode-sam/sam/sam_to_mgrec/transcoder.h
#            transcode-sam/sam/sam_to_mgrec/sorter.cc
#            transcode-sam/sam/sam_to_mgrec/sorter.h
#            transcode-sam/sam/sam_to_mgrec/sam_reader.h
#            transcode-sam/sam/sam_to_mgrec/sam_reader.cc
#            transcode-sam/sam/sam_to_mgrec/sam_group.h
#            transcode-sam/sam/sam_to_mgrec/sam_group.cc
#            )
#endif ()

add_executable(genie ${source_files})

target_link_libraries(genie PRIVATE genie-util)
target_link_libraries(genie PRIVATE genie-module)
target_link_libraries(genie PRIVATE genie-gabac)
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

#if (${GENIE_USE_OPENMP})
#    target_compile_definitions(genie PRIVATE GENIE_USE_OPENMP)
#    target_link_libraries(genie PRIVATE ${OpenMP_CXX_LIBRARIES})
#endif ()
#
#if (${GENIE_SAM_SUPPORT})
#    target_compile_definitions(genie PRIVATE GENIE_SAM_SUPPORT)
#    target_link_libraries(genie ${LIBLZMA_LIBRARIES})
#    target_link_libraries(genie ${BZIP2_LIBRARIES})
#    target_link_libraries(genie ${ZLIB_LIBRARIES})
#
#    find_package(HTSlib REQUIRED)
#    target_include_directories(genie PRIVATE ${HTSlib_INCLUDE_DIRS})
#    target_link_libraries(genie PRIVATE ${HTSlib_LIBRARIES})
#endif ()

install(TARGETS genie
        RUNTIME DESTINATION "usr/bin")
