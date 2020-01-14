project("gabac")

set(source_files
        analysis.cc
        # binary-arithmetic-decoder.cc
        # binary-arithmetic-encoder.cc
        bit-input-stream.cc
        c-interface.cc
        configuration.cc
        constants.cc
        context-model.cc
        context-tables.cc
        data-block.cc
        decode-cabac.cc
        decoding.cc
        diff-coding.cc
        encode-cabac.cc
        encoding.cc
        equality-coding.cc
        exceptions.cc
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
    add_library(gabac SHARED ${source_files})
else ()
    add_library(gabac STATIC ${source_files})
endif ()

target_link_libraries(gabac PRIVATE util)
