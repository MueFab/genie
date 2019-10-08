project("encapsulator")

set(source_files
        main.cc
        program-options.cc
        FileHandlingUtlis.cc
        Boxes.c
        vector.c
        ClassesSet.c
        treeUint64.c
        VectorUint64.c
        utils.c
        DataStructures/FromFile.c
        DataStructures/EncodingParameters.c
        DataStructures/Signatures/Signature.c
        DataStructures/BitStreams/InputBitstream.c
        DataStructures/BitStreams/OutputBitstream.c
        DataUnits/DataUnits.c)

add_executable(encapsulator ${source_files})
