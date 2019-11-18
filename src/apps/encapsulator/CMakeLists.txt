#add_subdirectory(BoxesDefinitions)
#add_subdirectory(DataStructures)
#add_subdirectory(DataUnits)

project("encapsulator")

set(source_files
#        Boxes.c
#        ClassesSet.c
#        FastaReader.c
        FileHandlingUtils.cc
        main.cc
        program-options.cc
#        treeUint64.c
#        utils.c
#        vector.c
#        VectorUint64.c
#
#        BoxesDefinitions/AccessUnit/AccessUnitContainer.c
#        BoxesDefinitions/AccessUnit/AccessUnitHeader.c
#        BoxesDefinitions/AccessUnit/AccessUnitInformation.c
#        BoxesDefinitions/AccessUnit/AccessUnitProtection.c
#        BoxesDefinitions/Block/BlockContainer.c
#        BoxesDefinitions/Block/BlockHeader.c
#        BoxesDefinitions/Dataset/DatasetContainer.c
#        BoxesDefinitions/Dataset/DatasetHeader.c
#        BoxesDefinitions/Dataset/DatasetMasterIndexTable.c
#        BoxesDefinitions/Dataset/DatasetMetadata.c
#        BoxesDefinitions/Dataset/DatasetParameters.c
#        BoxesDefinitions/Dataset/DatasetProtection.c
#        BoxesDefinitions/DatasetsGroup/DatasetsGroupContainer.c
#        BoxesDefinitions/DatasetsGroup/DatasetsGroupHeader.c
#        BoxesDefinitions/DatasetsGroup/DatasetsGroupLabel.c
#        BoxesDefinitions/DatasetsGroup/DatasetsGroupMetadata.c
#        BoxesDefinitions/DatasetsGroup/DatasetsGroupProtection.c
#        BoxesDefinitions/DatasetsGroup/DatasetsGroupReferenceGenome.c
#        BoxesDefinitions/DatasetsGroup/DatasetsGroupReferenceMetadata.c
#        BoxesDefinitions/File/File.c
#        BoxesDefinitions/File/FileHeader.c
#        BoxesDefinitions/Stream/StreamContainer.c
#        BoxesDefinitions/Stream/StreamHeader.c
#        BoxesDefinitions/Stream/StreamProtection.c
#
#        DataStructures/ByteArray.c
#        DataStructures/EncodingParameters.c
#        DataStructures/FromFile.c
#        DataStructures/Label.c
#        DataStructures/BitStreams/InputBitstream.c
#        DataStructures/BitStreams/OutputBitstream.c
#        DataStructures/Signatures/Signature.c
#        DataStructures/Signatures/Signatures.c
#        DataStructures/Signatures/SignatureSizeComputation.c
#
#        DataUnits/DataUnitAccessUnit.c
#        DataUnits/DataUnitBlockHeader.c
#        DataUnits/DataUnitRawReference.c
#        DataUnits/DataUnits.c
#        DataUnits/DataUnitsParametersSet.c
        )

add_executable(encapsulator ${source_files})

target_link_libraries(encapsulator PRIVATE util)
target_link_libraries(encapsulator PRIVATE format)

#target_link_libraries(encapsulator PRIVATE encapsulatorBoxesDefinitions)
#target_link_libraries(encapsulator PRIVATE encapsulatorDataStructures)
#target_link_libraries(encapsulator PRIVATE encapsulatorDataUnits)