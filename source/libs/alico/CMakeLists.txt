project("alico")

set(source_files
        read_compression.cc
        util.cc
        quantizer.cc
        read_decompression.cc
        id_compression.cc
        Arithmetic_stream.cc
        sam_file_allocation.cc
        distortion.cc
        compression.cc
        sfio_ssh_session.cc
        qv_codebook.cc
        main.cc
        edit.cc
        aux_compression.cc
        pmf.cc
        stream_model.cc
        io_functions.cc
        qv_compression.cc
        well.cc
        sam_models.cc
)

add_library(alico ${source_files})
target_include_directories(alico PRIVATE "${CMAKE_SOURCE_DIR}/source/coding/alico")
target_include_directories(alico PRIVATE "${CMAKE_SOURCE_DIR}/source/coding")
target_link_libraries(alico calq)
target_link_libraries(alico pthread)
