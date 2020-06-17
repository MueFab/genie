project("genie-spring")

set(source_files
        bitset-util.cc
        call-template-functions.cc
        decoder.cc
        spring-encoding.cc
        generate-read-streams.cc
        preprocess.cc
        reorder-compress-quality-id.cc
        encoder.cc
        encoder-source.cc
        util.cc
        )

add_library(genie-spring ${source_files})

target_link_libraries(genie-spring PUBLIC genie-gabac)

target_link_libraries(genie-spring PUBLIC genie-core)

if (${GENIE_USE_OPENMP})
    target_compile_definitions(genie-spring PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(genie-spring PRIVATE ${OpenMP_CXX_LIBRARIES})
endif ()
