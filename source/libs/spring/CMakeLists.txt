project("spring")

set(source_files
        bitset-util.cc
        call-template-functions.cc
        compress-ureads.cc
        decompress.cc
        encoder.cc
        generate-new-fastq.cc
        generate-read-streams.cc
        id-tokenization.cc
        preprocess.cc
        reorder-compress-quality-id.cc
        return-structures.cc
        spring.cc
        util.cc
)

add_library(spring ${source_files})

target_link_libraries(spring util)

if(${GENIE_USE_OPENMP})
    find_package(OpenMP REQUIRED)
    target_compile_definitions(spring PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(spring ${OpenMP_CXX_LIBRARIES})
endif()
