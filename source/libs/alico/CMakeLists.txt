project("alico")

set(source_files
        Arithmetic_stream.cc
        aux_compression.cc
        compression.cc
        distortion.cc
        edit.cc
        id_compression.cc
        io_functions.cc
        main.cc
        pmf.cc
        quantizer.cc
        qv_codebook.cc
        qv_compression.cc
        read_compression.cc
        read_decompression.cc
        sam_file_allocation.cc
        sam_models.cc
        sfio_ssh_session.cc
        stream_model.cc
        util.cc
        well.cc
)

add_library(alico ${source_files})

target_link_libraries(alico PRIVATE calq)
