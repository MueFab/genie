project("encapsulator")

set(source_files
        Boxes.c
        ClassesSet.c
        FastaReader.c
        FileHandlingUtlis.cc
        main.cc
        program-options.cc
        treeUint64.c
        utils.c
        vector.c
        VectorUint64.c
#        BoxesDefinitions/Block/BlockContainer.c
#        BoxesDefinitions/Block/BlockHeader.c
#        BoxesDefinitions/Dataset/DatasetContainer.c
#        BoxesDefinitions/Dataset/DatasetHeader.c
#        BoxesDefinitions/Dataset/DatasetMasterIndexTable.c
#        BoxesDefinitions/Dataset/DatasetMetadata.c
#        BoxesDefinitions/Dataset/DatasetParameters.c
#        BoxesDefinitions/Dataset/DatasetProtection.c
#        DataStructures/ByteArray.c
#        DataStructures/EncodingParameters.c
#        DataStructures/FromFile.c
#        DataStructures/Label.c
#        DataStructures/BitStreams/InputBitstream.c
#        DataStructures/BitStreams/OutputBitstream.c
#        DataUnits/DataUnitAccessUnit.c
#        DataUnits/DataUnitBlockHeader.c
#        DataUnits/DataUnitRawReference.c
#        DataUnits/DataUnits.c
#        DataUnits/DataUnitsParametersSet.c
)

add_executable(encapsulator ${source_files})
