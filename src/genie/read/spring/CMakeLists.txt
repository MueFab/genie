project("genie-spring")

set(source_files
        bitset-util.cc
        call-template-functions.cc
        combine-aus.cc
        compress-ureads.cc
        decompress.cc
        encoder.cc
        generate-new-fastq.cc
        generate-read-streams.cc
        preprocess.cc
        reorder-compress-quality-id.cc
        return-structures.cc
        spring.cc
        spring-encoder.cc
        util.cc
        )

add_library(genie-spring ${source_files})

target_link_libraries(genie-spring PUBLIC genie-gabac)

target_link_libraries(genie-spring PUBLIC genie-core)

if (${GENIE_USE_OPENMP})
    target_compile_definitions(genie-spring PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(genie-spring PRIVATE ${OpenMP_CXX_LIBRARIES})
endif ()
