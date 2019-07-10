project("spring")

set(source_files
        ReturnStructures.cpp
        bitset_util.cpp
        decompress.cpp
        encoder.cpp
        generate_new_fastq.cpp
        generate_read_streams.cpp
        id_tokenization.cpp
        preprocess.cpp
        reorder_compress_quality_id.cpp
        spring.cpp
        util.cpp
        )

add_library(spring ${source_files})

if(${GENIE_USE_OPENMP})
    find_package(OpenMP REQUIRED)
    target_compile_definitions(spring PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(spring ${OpenMP_CXX_LIBRARIES})
endif()
