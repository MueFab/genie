project("calq")

set(source_files
        calq_coder.cc
        circular_buffer.h
        exceptions.cc
        filter_buffer.cc
        genotyper.cc
        haplotyper.cc
        lloyd_max_quantizer.cc
        log.cc
        probability_distribution.cc
        quality_decoder.cc
        quality_encoder.cc
        quantizer.cc
        soft_clip_spreader.cc
        uniform_min_max_quantizer.cc
        uniform_quantizer.cc
        decoder.cc
        encoder.cc
        record_pileup.cc
)

add_library(genie-calq ${source_files})

target_link_libraries(genie-calq PUBLIC genie-core)
target_link_libraries(genie-calq PUBLIC genie-paramqv1)

get_filename_component(TOP_DIR ../../ ABSOLUTE)
get_filename_component(THIS_DIR ./ ABSOLUTE)
target_include_directories(genie-calq PUBLIC "${THIS_DIR}")
target_include_directories(genie-calq PUBLIC "${TOP_DIR}")