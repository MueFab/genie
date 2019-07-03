project("calq")

set(source_files
        helpers.cc
        File.cc
        Genotyper.cc
        Exceptions.cc
        CQFile.cc
        SAMPileupDeque.cc
        range.cc
        SAMPileup.cc
        Quantizer.cc
        QualDecoder.cc
        SAMRecord.cc
        QualEncoder.cc
        UniformQuantizer.cc
        UniformMinMaxQuantizer.cc
        Options.cc
)

add_library(calq ${source_files})
target_include_directories(calq PRIVATE "${CMAKE_SOURCE_DIR}/source/coding/calq")
