project("genie-gabac")

set(source_files
        #analysis.cc
        bit-input-stream.cc
        c-interface.cc
        configuration.cc
        constants.cc
        context-model.cc
        context-tables.cc
        decode-cabac.cc
        decoding.cc
        diff-coding.cc
        encode-cabac.cc
        encoding.cc
        equality-coding.cc
        exceptions.cc
        gabac-compressor.cc
        gabac-decompressor.cc
        gabac-seq-conf-set.cc
        genie-gabac-output-stream.cc
        lut-transform.cc
        match-coding.cc
        reader.cc
        rle-coding.cc
        run.cc
        stream-handler.cc
        streams.cc
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