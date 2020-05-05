project("genie-gabac")

set(source_files
        binary-arithmetic-encoder.cc
        binary-arithmetic-decoder.cc
        bit-input-stream.cc
        config-manual.cc
        configuration.cc
        context-model.cc
        context-tables.cc
        exceptions.cc
        luts-subsymbol-transform.cc
        equality-subseq-transform.cc
        match-subseq-transform.cc
        merge-subseq-transform.cc
        rle-subseq-transform.cc
        decode-transformed-subseq.cc
        decode-desc-subseq.cc
        encode-transformed-subseq.cc
        encode-desc-subseq.cc
        gabac-compressor.cc
        gabac-decompressor.cc
        gabac-seq-conf-set.cc
        run.cc
        stream-handler.cc
        streams.cc
        reader.cc
        writer.cc
        )

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