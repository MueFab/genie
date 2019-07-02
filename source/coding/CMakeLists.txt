add_subdirectory(gabac)

#==============================================================================
# coding library
#==============================================================================

# Set our coding paths
set(coding "coding")
set(coding_source_dir ${CMAKE_SOURCE_DIR}/source/coding )
set(coding_header_dir ${CMAKE_SOURCE_DIR}/source/coding )
set(coding_include_dir ${CMAKE_SOURCE_DIR}/source/coding ${CMAKE_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/gabac-src/source)

# List all source files
set(coding_source_files ${coding_source_files} ${coding_source_dir}/ReturnStructures.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/spring.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/preprocess.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/util.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/bitset_util.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/encoder.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/reorder_compress_quality_id.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_read_streams.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_new_fastq.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/decompress.cpp)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/id_tokenization.cpp)

# List all header files
set(coding_source_files ${coding_source_files} ${coding_source_dir}/ReturnStructures.h)
set(coding_header_files ${coding_header_files} ${coding_header_dir}/CoderConstans.h)
set(coding_header_files ${coding_header_files} ${coding_header_dir}/spring.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/bitset_util.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/BooPHF.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/decompress.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/encoder.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_new_fastq.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/generate_read_streams.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/id_tokenization.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/params.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/preprocess.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/reorder_compress_quality_id.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/reorder.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/spring.h)
set(coding_source_files ${coding_source_files} ${coding_source_dir}/spring/util.h)

set(SAMStreamer "SAMStreamer")

set(sam_source_dir ${coding_source_dir}/alico/src/SAMStreamer)
set(sam_include_dir ${coding_source_dir}/alico/include)

set(calq_source_dir ${coding_source_dir}/alico/src/extern/calq)
set(calq_qualcodec_dir ${calq_source_dir}/QualCodec)
set(calq_common_dir ${calq_source_dir}/Common)

message(STATUS "sam  source dir: ${sam_source_dir}")
message(STATUS "sam include dir: ${sam_include_dir}")

set(sam_source_files ${sam_source_files} ${sam_source_dir}/Arithmetic_stream.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/aux_compression.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/compression.cc)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/distortion.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/edit.cc)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/id_compression.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/io_functions.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/main.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/pmf.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/quantizer.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/qv_codebook.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/qv_compression.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/read_compression.cc)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/read_decompression.cc)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/sam_file_allocation.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/sam_models.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/sfio_ssh_session.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/stream_model.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/util.c)
set(sam_source_files ${sam_source_files} ${sam_source_dir}/well.c)

SET_SOURCE_FILES_PROPERTIES(${sam_source_files} PROPERTIES LANGUAGE CXX )


set(sam_header_files ${sam_header_files} ${sam_include_dir}/Arithmetic_stream.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/aux_data.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/distortion.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/edit.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/id_compression.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/pmf.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/quantizer.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/qv_codebook.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/read_compression.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/sam_block.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/stream_model.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/util.h)
set(sam_header_files ${sam_header_files} ${sam_include_dir}/well.h)

set(calq_source_files ${calq_source_files} ${calq_source_dir}/IO/SAM/SAMPileupDeque.cc)
set(calq_source_files ${calq_source_files} ${calq_source_dir}/IO/SAM/SAMPileup.cc)
set(calq_source_files ${calq_source_files} ${calq_source_dir}/IO/SAM/SAMRecord.cc)

set(calq_header_files ${calq_header_files} ${calq_source_dir}/IO/SAM/SAMPileupDeque.h)
set(calq_header_files ${calq_source_files} ${calq_source_dir}/IO/SAM/SAMPileup.h)
set(calq_header_files ${calq_header_files} ${calq_source_dir}/IO/SAM/SAMRecord.h)



set(calq_source_files ${calq_source_files} ${calq_source_dir}/IO/File.cc)
set(calq_source_files ${calq_source_files} ${calq_source_dir}/IO/CQ/CQFile.cc)
set(calq_source_files ${calq_source_files} ${calq_source_dir}/Compressors/range/range.cc)


set(calq_source_files ${calq_source_files} ${calq_qualcodec_dir}/Genotyper.cc)
set(calq_source_files ${calq_source_files} ${calq_qualcodec_dir}/QualDecoder.cc)
set(calq_source_files ${calq_source_files} ${calq_qualcodec_dir}/QualEncoder.cc)
set(calq_source_files ${calq_source_files} ${calq_qualcodec_dir}/Quantizers/Quantizer.cc)
set(calq_source_files ${calq_source_files} ${calq_qualcodec_dir}/Quantizers/UniformMinMaxQuantizer.cc)
set(calq_source_files ${calq_source_files} ${calq_qualcodec_dir}/Quantizers/UniformQuantizer.cc)

set(calq_header_files ${calq_header_files} ${calq_qualcodec_dir}/Genotyper.h)
set(calq_header_files ${calq_header_files} ${calq_qualcodec_dir}/QualDecoder.h)
set(calq_header_files ${calq_header_files} ${calq_qualcodec_dir}/QualEncoder.h)
set(calq_header_files ${calq_header_files} ${calq_qualcodec_dir}/Quantizers/UniformMinMaxQuantizer.h)
set(calq_header_files ${calq_header_files} ${calq_qualcodec_dir}/Quantizers/UniformQuantizer.h)

set(calq_source_files ${calq_source_files} ${calq_common_dir}/Exceptions.cc)
set(calq_source_files ${calq_source_files} ${calq_common_dir}/helpers.cc)
set(calq_source_files ${calq_source_files} ${calq_common_dir}/Options.cc)

set(calq_header_files ${calq_header_files} ${calq_common_dir}/constants.h)
set(calq_header_files ${calq_header_files} ${calq_common_dir}/Exceptions.h)
set(calq_header_files ${calq_header_files} ${calq_common_dir}/helpers.h)
set(calq_header_files ${calq_header_files} ${calq_common_dir}/log.h)
set(calq_header_files ${calq_header_files} ${calq_common_dir}/Options.h)
set(calq_header_files ${calq_header_files} ${calq_common_dir}/os.h)


SET_SOURCE_FILES_PROPERTIES(${sam_header_files} PROPERTIES LANGUAGE CXX )

# Group the source and header files
source_group("Source Files" FILES ${coding_source_files} ${calq_source_files} ${sam_source_files})
source_group("Header Files" FILES ${coding_header_files} ${calq_header_files} ${sam_header_files})

# Set up the format target
add_library(${coding} STATIC ${coding_source_files} ${calq_source_files} ${sam_source_files})
target_include_directories(${coding} PRIVATE ${coding_include_dir} ${sam_include_dir} ${calq_source_dir})
