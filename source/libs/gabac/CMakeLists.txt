project("gabac")

set(source_files
        analysis.cpp
        binary_arithmetic_decoder.cpp
        # binary_arithmetic_encoder.cpp
        bit_input_stream.cpp
        bit_output_stream.cpp
        c_interface.cpp
        configuration.cpp
        constants.cpp
        context_model.cpp
        context_tables.cpp
        data_block.cpp
        decode_cabac.cpp
        decoding.cpp
        diff_coding.cpp
        encode_cabac.cpp
        encoding.cpp
        equality_coding.cpp
        exceptions.cpp
        lut_transform.cpp
        match_coding.cpp
        reader.cpp
        rle_coding.cpp
        run.cpp
        stream_handler.cpp
        streams.cpp
        writer.cpp
)

add_library(gabac ${source_files})
