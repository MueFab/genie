cmake_minimum_required(VERSION 3.1)


project("SAMStreamer" LANGUAGES CXX)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Detected compiler: GNU Compiler Collection (${CMAKE_CXX_COMPILER})")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-prototypes")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Waggregate-return")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")

    message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "Detected compiler: LLVM Clang or Apple Clang")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Detected compiler: Microsoft Visual Studio")

    message(FATAL_ERROR "Compiler currently not supported by CMake build: ${CMAKE_CXX_COMPILER_ID}")
else ()
    message(FATAL_ERROR "Compiler currently not supported by CMake build: ${CMAKE_CXX_COMPILER_ID}")
endif ()


set(SAMStreamer "SAMStreamer")

set(sam_source_dir ${CMAKE_SOURCE_DIR}/src/SAMStreamer)
set(sam_include_dir ${CMAKE_SOURCE_DIR}/include)

set(calq_source_dir ${CMAKE_SOURCE_DIR}/src/extern/calq)
set(calq_qualcodec_dir ${calq_source_dir}/QualCodec)
set(calq_common_dir ${calq_source_dir}/Common)

message(STATUS "sam  source dir: ${sam_source_dir}")
message(STATUS "sam include dir: ${sam_include_dir}")
message(STATUS "sam     ssh dir: ${sam_include_ssh_dir}")

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


# link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)
# include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_executable(SAMStreamer ${sam_source_files} ${calq_source_files})
target_include_directories(${SAMStreamer} PRIVATE ${sam_include_dir})
target_include_directories(${SAMStreamer} PRIVATE ${calq_source_dir})

target_link_libraries(${SAMStreamer} ssh pthread)

# set(encoder "encoder")
# set(encoder_source_dir ${CMAKE_SOURCE_DIR}/source/encoder)
# set(encoder_include_dir ${CMAKE_SOURCE_DIR}/source/encoder)

# set(encoder_source_files ${encoder_source_files} ${encoder_source_dir}/encoder.cc)

# #set(encoder_header_files ${encoder_header_files} )

# add_executable(encoder ${encoder_source_files})

# target_include_directories(${encoder} PRIVATE ${encoder_include_dir})
