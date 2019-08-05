project("spring")

set(source_files
        return-structures.cc
        bitset-util.cc
        decompress.cc
        encoder.cc
        generate-new-fastq.cc
        generate-read-streams.cc
        id-tokenization.cc
        preprocess.cc
        compress-ureads.cc
        reorder-compress-quality-id.cc
        spring.cc
        util.cc
        )

add_library(spring ${source_files})

if(${GENIE_USE_OPENMP})
    find_package(OpenMP REQUIRED)
    target_compile_definitions(spring PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(spring ${OpenMP_CXX_LIBRARIES})
endif()
