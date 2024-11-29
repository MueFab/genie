project("genie-gabac")

set(source_files
        bit_input_stream.cc
        bit_input_stream.h
        config_manual.cc
        configuration.cc
        context_model.cc
        context_selector.cc
        context_tables.cc
        luts_sub_symbol_transform.cc
        equality_sub_seq_transform.cc
        match_sub_seq_transform.cc
        mismatch_decoder.cc
        merge_sub_seq_transform.cc
        rle_sub_seq_transform.cc
        decode_transformed_sub_seq.cc
        decode_transformed_symbols.cc
        decode_desc_sub_seq.cc
        decoder.cc
        encode_transformed_sub_seq.cc
        encode_desc_sub_seq.cc
        encoder.cc
        gabac_seq_conf_set.cc
        run.cc
        stream_handler.cc
        streams.cc
        reader.cc
        writer.cc
        benchmark.h benchmark.cc benchmark.impl.h
        gabac_seq_conf_set.impl.h)

if (${GABAC_BUILD_SHARED_LIB})
    add_library(genie-gabac SHARED ${source_files})
else ()
    add_library(genie-gabac STATIC ${source_files})
endif ()

target_link_libraries(genie-gabac PUBLIC genie-core)
target_link_libraries(genie-gabac PUBLIC genie-paramcabac)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-gabac PUBLIC "${THIS_DIR}")
target_include_directories(genie-gabac PUBLIC "${TOP_DIR}")