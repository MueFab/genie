project("encapsulatorBoxesDefinitions")

set(source_files
        AccessUnit/AccessUnitContainer.c
        AccessUnit/AccessUnitHeader.c
        AccessUnit/AccessUnitInformation.c
        AccessUnit/AccessUnitProtection.c
        Block/BlockContainer.c
        Block/BlockHeader.c
        Dataset/DatasetContainer.c
        Dataset/DatasetHeader.c
        Dataset/DatasetMasterIndexTable.c
        Dataset/DatasetMetadata.c
        Dataset/DatasetParameters.c
        Dataset/DatasetProtection.c
        DatasetsGroup/DatasetsGroupContainer.c
        DatasetsGroup/DatasetsGroupHeader.c
        DatasetsGroup/DatasetsGroupLabel.c
        DatasetsGroup/DatasetsGroupMetadata.c
        DatasetsGroup/DatasetsGroupProtection.c
        DatasetsGroup/DatasetsGroupReferenceGenome.c
        DatasetsGroup/DatasetsGroupReferenceMetadata.c
        Stream/StreamContainer.c
        Stream/StreamHeader.c
        Stream/StreamProtection.c
)

add_library(encapsulatorBoxesDefinitions ${source_files})