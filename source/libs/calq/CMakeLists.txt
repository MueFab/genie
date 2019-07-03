project("calq")

set(source_files
        CQFile.cc
        Exceptions.cc
        File.cc
        Genotyper.cc
        Options.cc
        QualDecoder.cc
        QualEncoder.cc
        Quantizer.cc
        SAMPileup.cc
        SAMPileupDeque.cc
        SAMRecord.cc
        UniformMinMaxQuantizer.cc
        UniformQuantizer.cc
        helpers.cc
        range.cc
)

add_library(calq ${source_files})
