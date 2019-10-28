project("encapsulatorDataStructures")

set(source_files
        ByteArray.c
        EncodingParameters.c
        FromFile.c
        Label.c
        BitStreams/InputBitstream.c
        BitStreams/OutputBitstream.c
        Signatures/Signature.c
        Signatures/Signatures.c
        Signatures/SignatureSizeComputation.c
)

add_library(encapsulatorDataStructures ${source_files})