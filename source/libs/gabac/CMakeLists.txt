project("gabac")

set(source_files
        analysis.cc
        binary-arithmetic-decoder.cc
        # binary_arithmetic_encoder.cc
        bit-input-stream.cc
        bit-output-stream.cc
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

add_library(gabac ${source_files})
