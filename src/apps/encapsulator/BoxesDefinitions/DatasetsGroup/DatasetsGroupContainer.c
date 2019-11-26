//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../../Boxes.h"
#include "../../utils.h"

void freeDatasetsGroupReferenceMetadatas(Vector* referenceMetadata);
void freeDatasetsGroupReferenceGenomes(Vector* referenceGenomes);

DatasetsGroupContainer* initDatasetsGroupContainer() {
    DatasetsGroupContainer* datasetsGroupContainer = (DatasetsGroupContainer*)calloc(1, sizeof(DatasetsGroupContainer));
    datasetsGroupContainer->datasetsGroupHeader = NULL;
    datasetsGroupContainer->datasetsGroupReferenceGenomes = NULL;
    datasetsGroupContainer->datasetsGroupReferenceMetadatas = NULL;
    datasetsGroupContainer->datasetsGroupMetadata = NULL;
    datasetsGroupContainer->datasetsGroupProtection = NULL;
    datasetsGroupContainer->datasetsGroupLabelsList = NULL;
    datasetsGroupContainer->datasetsContainer = initVector();
    return datasetsGroupContainer;
}

uint64_t getSizeDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer) {
    return 12 + getSizeContentDatasetsGroupContainer(datasetsGroupContainer);
}

uint64_t getSizeContentDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer) {
    uint64_t contentSize = datasetsGroupContainer->datasetsGroupHeader != NULL
                               ? getSizeDatasetsGroupHeader(datasetsGroupContainer->datasetsGroupHeader)
                               : 0;

    if (datasetsGroupContainer->datasetsGroupReferenceGenomes != NULL) {
        for (size_t reference_i = 0; reference_i < getSize(datasetsGroupContainer->datasetsGroupReferenceGenomes);
             reference_i++) {
            DatasetsGroupReferenceGenome* referenceGenome =
                getValue(datasetsGroupContainer->datasetsGroupReferenceGenomes, reference_i);
            contentSize += referenceGenome != NULL ? getDatasetsGroupReferenceGenomeSize(referenceGenome) : 0;
        }
    }

    if (datasetsGroupContainer->datasetsGroupReferenceMetadatas != NULL) {
        for (size_t referenceMetadata_i = 0;
             referenceMetadata_i < getSize(datasetsGroupContainer->datasetsGroupReferenceMetadatas);
             referenceMetadata_i++) {
            DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata =
                getValue(datasetsGroupContainer->datasetsGroupReferenceMetadatas, referenceMetadata_i);
            contentSize += datasetsGroupReferenceMetadata != NULL
                               ? getSizeDatasetsGroupReferenceMetadata(datasetsGroupReferenceMetadata)
                               : 0;
        }
    }

    contentSize += datasetsGroupContainer->datasetsGroupLabelsList != NULL
                       ? getSizeDatasetsGroupLabelsList(datasetsGroupContainer->datasetsGroupLabelsList)
                       : 0;
    size_t datasetsContainerSize = getSize(datasetsGroupContainer->datasetsContainer);
    for (size_t datasetContainer_i = 0; datasetContainer_i < datasetsContainerSize; datasetContainer_i++) {
        DatasetContainer* datasetContainer = getValue(datasetsGroupContainer->datasetsContainer, datasetContainer_i);
        contentSize += datasetContainer != NULL ? getSizeDatasetContainer(datasetContainer) : 0;
    }
    contentSize += datasetsGroupContainer->datasetsGroupMetadata != NULL
                       ? getSizeDatasetsGroupMetadata(datasetsGroupContainer->datasetsGroupMetadata)
                       : 0;
    contentSize += datasetsGroupContainer->datasetsGroupProtection != NULL
                       ? getDatasetsGroupProtectionSize(datasetsGroupContainer->datasetsGroupProtection)
                       : 0;
    return contentSize;
}

bool writeDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer, FILE* outputFile) {
    uint64_t datasetsGroupSize = getSizeDatasetsGroupContainer(datasetsGroupContainer);
    bool boxHeaderSuccessfulWrite = writeBoxHeader(outputFile, datasetsGroupBoxName, datasetsGroupSize);
    if (!boxHeaderSuccessfulWrite) {
        fprintf(stderr, "Error writing type or size of dataset group container.\n");
        return false;
    }
    return writeContentDatasetsGroupContainer(datasetsGroupContainer, outputFile);
}

bool writeContentDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer, FILE* outputFile) {
    bool datasetsGroupHeaderSuccessfulWrite = true;
    if (datasetsGroupContainer->datasetsGroupHeader != NULL) {
        datasetsGroupHeaderSuccessfulWrite =
            writeDatasetsGroupHeader(datasetsGroupContainer->datasetsGroupHeader, outputFile);
    }
    bool datasetsGroupReferenceGenomesSuccessfulWrite = true;
    if (datasetsGroupContainer->datasetsGroupReferenceGenomes != NULL) {
        for (size_t reference_i = 0; reference_i < getSize(datasetsGroupContainer->datasetsGroupReferenceGenomes);
             reference_i++) {
            DatasetsGroupReferenceGenome* referenceGenome =
                getValue(datasetsGroupContainer->datasetsGroupReferenceGenomes, reference_i);
            if (referenceGenome != NULL) {
                if (!writeDatasetsGroupReferenceGenome(referenceGenome, outputFile)) {
                    datasetsGroupReferenceGenomesSuccessfulWrite = false;
                };
            }
        }
    }
    bool datasetsGroupReferenceMetadatasSuccessfulWrite = true;
    if (datasetsGroupContainer->datasetsGroupReferenceMetadatas != NULL) {
        for (size_t referenceMetadata_i = 0;
             referenceMetadata_i < getSize(datasetsGroupContainer->datasetsGroupReferenceMetadatas);
             referenceMetadata_i++) {
            DatasetsGroupReferenceMetadata* referenceMetadata =
                getValue(datasetsGroupContainer->datasetsGroupReferenceMetadatas, referenceMetadata_i);
            if (referenceMetadata != NULL) {
                if (!writeDatasetsGroupReferenceMetadata(referenceMetadata, outputFile)) {
                    datasetsGroupReferenceMetadatasSuccessfulWrite = false;
                    break;
                }
            }
        }
    }
    if (!datasetsGroupHeaderSuccessfulWrite || !datasetsGroupReferenceGenomesSuccessfulWrite ||
        !datasetsGroupReferenceMetadatasSuccessfulWrite) {
        fprintf(stderr, "Error writing datasets group container.\n");
        return false;
    }

    if (datasetsGroupContainer->datasetsGroupLabelsList != NULL) {
        if (!writeDatasetsGroupLabelsList(datasetsGroupContainer->datasetsGroupLabelsList, outputFile)) {
            fprintf(stderr, "Error writing datasets group container.\n");
            return false;
        }
    }

    bool datasetsGroupMetadataSuccessfulWrite = true;
    if (datasetsGroupContainer->datasetsGroupMetadata != NULL) {
        datasetsGroupMetadataSuccessfulWrite =
            writeDatasetsGroupMetadata(datasetsGroupContainer->datasetsGroupMetadata, outputFile);
    }
    bool datasetsGroupProtectionSuccessfulWrite = true;
    if (datasetsGroupContainer->datasetsGroupProtection != NULL) {
        datasetsGroupProtectionSuccessfulWrite =
            writeDatasetsGroupProtection(datasetsGroupContainer->datasetsGroupProtection, outputFile);
    }
    if (!datasetsGroupMetadataSuccessfulWrite || !datasetsGroupProtectionSuccessfulWrite) {
        fprintf(stderr, "Error writing datasets group container.\n");
        return false;
    }

    size_t datasetsContainerSize = getSize(datasetsGroupContainer->datasetsContainer);
    for (size_t datasetContainer_i = 0; datasetContainer_i < datasetsContainerSize; datasetContainer_i++) {
        DatasetContainer* datasetContainer = getValue(datasetsGroupContainer->datasetsContainer, datasetContainer_i);
        bool datasetContainerSuccessfulWrite = true;
        if (datasetContainer != NULL) {
            datasetContainerSuccessfulWrite = writeDatasetContainer(datasetContainer, outputFile);
        }
        if (!datasetContainerSuccessfulWrite) {
            fprintf(stderr, "Error writing dataset container %lu.\n", datasetContainer_i);
            return false;
        }
    }
    return true;
}

void setDatasetsGroupHeader(DatasetsGroupContainer* datasetsGroupContainer, DatasetsGroupHeader* datasetsGroupHeader) {
    if (datasetsGroupContainer->datasetsGroupHeader != NULL) {
        freeDatasetsGroupHeader(datasetsGroupContainer->datasetsGroupHeader);
    }
    datasetsGroupContainer->datasetsGroupHeader = datasetsGroupHeader;
}

void setDatasetsGroupReferenceGenomes(DatasetsGroupContainer* datasetsGroupContainer,
                                      Vector* datasetsGroupReferenceGenomes) {
    if (datasetsGroupContainer->datasetsGroupReferenceGenomes != NULL) {
        DatasetsGroupReferenceGenome* referenceGenome;
        for (size_t reference_i = 0; reference_i < getSize(datasetsGroupContainer->datasetsGroupReferenceGenomes);
             reference_i++) {
            referenceGenome = getValue(datasetsGroupContainer->datasetsGroupReferenceGenomes, reference_i);
            if (referenceGenome != NULL) {
                freeDatasetsGroupReferenceGenome(referenceGenome);
            }
        }
    }
    datasetsGroupContainer->datasetsGroupReferenceGenomes = datasetsGroupReferenceGenomes;
}

void freeDatasetsGroupReferenceMetadatas(Vector* referenceMetadata) {
    for (size_t referenceMetadata_i = 0; referenceMetadata_i < getSize(referenceMetadata); referenceMetadata_i++) {
        DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata =
            getValue(referenceMetadata, referenceMetadata_i);
        if (datasetsGroupReferenceMetadata != NULL) {
            freeDatasetsGroupReferenceMetadata(datasetsGroupReferenceMetadata);
        }
        freeVector(referenceMetadata);
    }
}

void setDatasetsGroupReferenceMetadata(DatasetsGroupContainer* datasetsGroupContainer,
                                       Vector* datasetsGroupReferenceMetadatas) {
    if (datasetsGroupContainer->datasetsGroupReferenceMetadatas != NULL) {
        freeDatasetsGroupReferenceMetadatas(datasetsGroupContainer->datasetsGroupReferenceMetadatas);
    }
    datasetsGroupContainer->datasetsGroupReferenceMetadatas = datasetsGroupReferenceMetadatas;
}

void setDatasetsGroupMetadata(DatasetsGroupContainer* datasetsGroupContainer,
                              DatasetsGroupMetadata* datasetsGroupMetadata) {
    if (datasetsGroupContainer->datasetsGroupMetadata != NULL) {
        freeDatasetsGroupMetadata(datasetsGroupContainer->datasetsGroupMetadata);
    }
    datasetsGroupContainer->datasetsGroupMetadata = datasetsGroupMetadata;
}

void setDatasetsGroupProtection(DatasetsGroupContainer* datasetsGroupContainer,
                                DatasetsGroupProtection* datasetsGroupProtection) {
    if (datasetsGroupContainer->datasetsGroupProtection != NULL) {
        freeDatasetsGroupProtection(datasetsGroupContainer->datasetsGroupProtection);
    }
    datasetsGroupContainer->datasetsGroupProtection = datasetsGroupProtection;
}

void setDatasetsGroupLabelsList(DatasetsGroupContainer* datasetsGroupContainer,
                                DatasetsGroupLabelsList* datasetsGroupLabelsList) {
    if (datasetsGroupContainer->datasetsGroupLabelsList != NULL) {
        freeDatasetsGroupLabelsList(datasetsGroupContainer->datasetsGroupLabelsList);
    }
    datasetsGroupContainer->datasetsGroupLabelsList = datasetsGroupLabelsList;
}

DatasetsGroupLabelsList* getDatasetsGroupLabelsList(DatasetsGroupContainer* datasetsGroupContainer) {
    return datasetsGroupContainer->datasetsGroupLabelsList;
}

void addDatasetsContainer(DatasetsGroupContainer* datasetsGroupContainer, DatasetContainer* datasetContainer) {
    pushBack(datasetsGroupContainer->datasetsContainer, datasetContainer);
    addDatasetId(datasetsGroupContainer->datasetsGroupHeader, datasetContainer->datasetHeader->datasetId);
}

void freeDatasetsGroupReferenceGenomes(Vector* referenceGenomes) {
    for (size_t reference_i = 0; reference_i < getSize(referenceGenomes); reference_i++) {
        DatasetsGroupReferenceGenome* referenceGenome = getValue(referenceGenomes, reference_i);
        if (referenceGenome != NULL) {
            freeDatasetsGroupReferenceGenome(referenceGenome);
        }
    }
    freeVector(referenceGenomes);
}

void freeDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer) {
    if (datasetsGroupContainer->datasetsGroupHeader != NULL) {
        freeDatasetsGroupHeader(datasetsGroupContainer->datasetsGroupHeader);
    };
    if (datasetsGroupContainer->datasetsGroupReferenceGenomes != NULL) {
        freeDatasetsGroupReferenceGenomes(datasetsGroupContainer->datasetsGroupReferenceGenomes);
    };
    if (datasetsGroupContainer->datasetsGroupReferenceMetadatas != NULL) {
        freeDatasetsGroupReferenceMetadatas(datasetsGroupContainer->datasetsGroupReferenceMetadatas);
    }
    if (datasetsGroupContainer->datasetsGroupMetadata != NULL) {
        freeDatasetsGroupMetadata(datasetsGroupContainer->datasetsGroupMetadata);
    };
    if (datasetsGroupContainer->datasetsGroupProtection != NULL) {
        freeDatasetsGroupProtection(datasetsGroupContainer->datasetsGroupProtection);
    };
    if (datasetsGroupContainer->datasetsGroupLabelsList != NULL) {
        freeDatasetsGroupLabelsList(datasetsGroupContainer->datasetsGroupLabelsList);
    }

    size_t datasetsContainerSize = getSize(datasetsGroupContainer->datasetsContainer);
    for (size_t datasetContainer_i = 0; datasetContainer_i < datasetsContainerSize; datasetContainer_i++) {
        DatasetContainer* datasetContainer = getValue(datasetsGroupContainer->datasetsContainer, datasetContainer_i);
        if (datasetContainer != NULL) {
            freeDatasetContainer(datasetContainer);
        }
    }
    freeVector(datasetsGroupContainer->datasetsContainer);

    free(datasetsGroupContainer);
}

bool isDatasetsGroupContainerValid(DatasetsGroupContainer* datasetsGroupContainer) {
    bool valid = datasetsGroupContainer->datasetsGroupHeader != NULL &&
                 datasetsGroupContainer->datasetsGroupReferenceGenomes != NULL;
    if (!valid) {
        return false;
    }
    size_t datasetsContainerSize = getSize(datasetsGroupContainer->datasetsContainer);
    for (size_t datasetContainer_i = 0; datasetContainer_i < datasetsContainerSize; datasetContainer_i++) {
        DatasetContainer* datasetContainer = getValue(datasetsGroupContainer->datasetsContainer, datasetContainer_i);
        if (datasetContainer != NULL) {
            if (!isDatasetContainerValid(datasetContainer)) {
                return false;
            }
        }
    }
    return true;
}

DatasetsGroupContainer* parseDatasetsGroupContainer(FILE* inputFile, uint64_t boxContentSize, char* filename) {
    enum PreviousState {
        init,
        dataset_group_header,
        reference,
        reference_metadata,
        label_list,
        dataset,
        dataset_group_metadata,
        dataset_group_protection
    } previousState = init;
    DatasetsGroupContainer* datasetsGroupContainer = initDatasetsGroupContainer();
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position");
        freeDatasetsGroupContainer(datasetsGroupContainer);
        return NULL;
    }
    datasetsGroupContainer->seekPosition = (size_t)ftell(inputFile);

    while (getSizeContentDatasetsGroupContainer(datasetsGroupContainer) < boxContentSize) {
        char boxType[5];
        uint64_t boxSize;
        bool successfulRead = readBoxHeader(inputFile, boxType, &boxSize);
        if (successfulRead) {
            if (strncmp(boxType, datasetsGroupHeaderName, 4) == 0) {
                if (previousState != init) {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
                previousState = dataset_group_header;

                DatasetsGroupHeader* datasetsGroupHeader = parseDatasetsGroupHeader(boxSize - 12, inputFile);
                if (datasetsGroupHeader != NULL) {
                    setDatasetsGroupHeader(datasetsGroupContainer, datasetsGroupHeader);
                } else {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
            } else if (strncmp(boxType, datasetsGroupMetadataName, 4) == 0) {
                if (!(previousState == dataset_group_header || previousState == reference ||
                      previousState == reference_metadata || previousState == label_list)) {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
                previousState = dataset_group_metadata;
                DatasetsGroupMetadata* datasetsGroupMetadata = parseDatasetsGroupMetadata(boxSize - 12, inputFile);
                if (datasetsGroupMetadata != NULL) {
                    setDatasetsGroupMetadata(datasetsGroupContainer, datasetsGroupMetadata);
                } else {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
            } else if (strncmp(boxType, datasetsGroupProtectionName, 4) == 0) {
                if (!(previousState == dataset_group_header || previousState == reference ||
                      previousState == reference_metadata || previousState == label_list ||
                      previousState == dataset_group_metadata)) {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
                previousState = dataset_group_protection;
                DatasetsGroupProtection* datasetsGroupProtection =
                    parseDatasetsGroupProtection(boxSize - 12, inputFile);
                if (datasetsGroupProtection != NULL) {
                    setDatasetsGroupProtection(datasetsGroupContainer, datasetsGroupProtection);
                } else {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
            } else if (strncmp(boxType, datasetsGroupReferencesName, 4) == 0) {
                if (!(previousState == dataset_group_header || previousState == reference)) {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
                previousState = reference;

                Vector* referenceGenomes = datasetsGroupContainer->datasetsGroupReferenceGenomes;
                if (referenceGenomes == NULL) {
                    referenceGenomes = initVector();
                    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceGenomes);
                }
                DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome =
                    parseDatasetsGroupReferenceGenome(inputFile);
                if (datasetsGroupReferenceGenome != NULL) {
                    pushBack(referenceGenomes, datasetsGroupReferenceGenome);
                } else {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
            } else if (strncmp(boxType, datasetsGroupReferenceMetadataName, 4) == 0) {
                if (!(previousState == reference || previousState == reference_metadata)) {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
                previousState = reference_metadata;

                Vector* referenceMetadatas = datasetsGroupContainer->datasetsGroupReferenceMetadatas;
                if (referenceMetadatas == NULL) {
                    referenceMetadatas = initVector();
                    setDatasetsGroupReferenceGenomes(datasetsGroupContainer, referenceMetadatas);
                }
                DatasetsGroupReferenceMetadata* referenceMetadata =
                    parseDatasetsGroupReferenceMetadata(boxSize - BOX_HEADER_SIZE, inputFile);
                if (referenceMetadata != NULL) {
                    pushBack(referenceMetadatas, referenceMetadata);
                } else {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
            } else if (strncmp(boxType, datasetContainerName, 4) == 0) {
                if (!(previousState == dataset_group_header || previousState == reference ||
                      previousState == reference_metadata || previousState == dataset || previousState == label_list ||
                      previousState == dataset_group_metadata || previousState == dataset_group_protection

                      )) {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
                previousState = dataset;
                DatasetContainer* datasetContainer = parseDatasetContainer(boxSize - 12, inputFile, filename);
                if (datasetContainer != NULL) {
                    addDatasetsContainer(datasetsGroupContainer, datasetContainer);
                } else {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                }
            } else if (strncmp(boxType, datasetsGroupLabelsListName, 4) == 0) {
                if (!(previousState == dataset_group_header || previousState == reference ||
                      previousState == reference_metadata)) {
                    freeDatasetsGroupContainer(datasetsGroupContainer);
                    return NULL;
                } else {
                    previousState = label_list;
                    DatasetsGroupLabelsList* datasetsGroupLabelsList = parseDatasetsGroupLabelsList(inputFile);
                    if (datasetsGroupLabelsList != NULL) {
                        setDatasetsGroupLabelsList(datasetsGroupContainer, datasetsGroupLabelsList);
                    } else {
                        freeDatasetsGroupContainer(datasetsGroupContainer);
                        return NULL;
                    }
                }
            } else {
                fprintf(stderr, "Unknown datasets group container element.\n");
                freeDatasetsGroupContainer(datasetsGroupContainer);
                return NULL;
            }
        } else {
            fprintf(stderr, "Could not read datasetsGroupContainer content.\n");
            freeDatasetsGroupContainer(datasetsGroupContainer);
            return NULL;
        }
    }

    return datasetsGroupContainer;
}

size_t getNumberDatasets(DatasetsGroupContainer* datasetsGroupContainer) {
    return getSize(datasetsGroupContainer->datasetsContainer);
}

DatasetContainer* getDatasetContainerByIndex(DatasetsGroupContainer* datasetsGroupContainer, size_t index) {
    return getValue(datasetsGroupContainer->datasetsContainer, index);
}

DatasetContainer* getDatasetContainerById(DatasetsGroupContainer* datasetsGroupContainer, DatasetId datasetId) {
    size_t numDatasets = getNumberDatasets(datasetsGroupContainer);
    for (size_t dataset_i = 0; dataset_i < numDatasets; dataset_i++) {
        DatasetContainer* datasetContainer = getDatasetContainerByIndex(datasetsGroupContainer, dataset_i);
        if (datasetContainer->datasetHeader->datasetId == datasetId) {
            return datasetContainer;
        }
    }
    return NULL;
}

size_t getDatasetGroupContainerSeekPosition(DatasetsGroupContainer* datasetsGroupContainer) {
    return datasetsGroupContainer->seekPosition;
}

bool generateDatasetsGroupSeekPoints(DatasetsGroupContainer* datasetsGroupContainer) {
    fprintf(stdout, "dataset group  @ %li - %li\n", getDatasetGroupContainerSeekPosition(datasetsGroupContainer),
            getDatasetGroupContainerSeekPosition(datasetsGroupContainer) +
                getSizeContentDatasetsGroupContainer(datasetsGroupContainer));

    if (datasetsGroupContainer->datasetsGroupHeader != NULL) {
        fprintf(stdout, "dataset group  header @ %li - %li\n",
                getDatasetGroupHeaderSeekPosition(datasetsGroupContainer->datasetsGroupHeader),
                getDatasetGroupHeaderSeekPosition(datasetsGroupContainer->datasetsGroupHeader) +
                    getSizeContentDatasetsGroupHeader(datasetsGroupContainer->datasetsGroupHeader));
    }
    if (datasetsGroupContainer->datasetsGroupReferenceGenomes != NULL) {
        fprintf(stdout, "dataset group  reference genomes\n");
        for (size_t reference_i = 0; reference_i < getSize(datasetsGroupContainer->datasetsGroupReferenceGenomes);
             reference_i++) {
            DatasetsGroupReferenceGenome* referenceGenome =
                getValue(datasetsGroupContainer->datasetsGroupReferenceGenomes, reference_i);
            if (referenceGenome != NULL) {
                getDatasetGroupReferenceGenomeSeekPosition(referenceGenome);

                fprintf(stdout, "dataset group  reference genome @ %li - %li\n",
                        getDatasetGroupReferenceGenomeSeekPosition(referenceGenome),
                        getDatasetGroupReferenceGenomeSeekPosition(referenceGenome) +
                            getDatasetsGroupReferenceGenomeContentSize(referenceGenome));
            }
        };
    }

    size_t datasetsContainerSize = getSize(datasetsGroupContainer->datasetsContainer);
    for (size_t datasetContainer_i = 0; datasetContainer_i < datasetsContainerSize; datasetContainer_i++) {
        DatasetContainer* datasetContainer = getValue(datasetsGroupContainer->datasetsContainer, datasetContainer_i);
        generateDatasetSeekPoints(datasetContainer);
    }

    if (datasetsGroupContainer->datasetsGroupMetadata != NULL) {
        fprintf(stdout, "dataset group  metadata @ %li - %li\n",
                getDatasetGroupMetadataSeekPosition(datasetsGroupContainer->datasetsGroupMetadata),
                getDatasetGroupMetadataSeekPosition(datasetsGroupContainer->datasetsGroupMetadata) +
                    getSizeContentDatasetsGroupMetadata(datasetsGroupContainer->datasetsGroupMetadata));
    }

    if (datasetsGroupContainer->datasetsGroupProtection != NULL) {
        fprintf(stdout, "dataset group  protection @ %li - %li\n",
                getDatasetGroupProtectionSeekPosition(datasetsGroupContainer->datasetsGroupProtection),
                getDatasetGroupProtectionSeekPosition(datasetsGroupContainer->datasetsGroupProtection) +
                    getDatasetsGroupProtectionContentSize(datasetsGroupContainer->datasetsGroupProtection));
    }

    return true;
}

DatasetsGroupReferenceGenome* getReferenceGenome(DatasetsGroupContainer* datasetsGroupContainer,
                                                 ReferenceId referenceId) {
    return getValue(datasetsGroupContainer->datasetsGroupReferenceGenomes, referenceId);
}