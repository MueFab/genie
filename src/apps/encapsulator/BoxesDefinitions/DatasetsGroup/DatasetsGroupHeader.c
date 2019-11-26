//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../../Boxes.h"
#include "../../utils.h"

DatasetsGroupHeader* initDatasetsGroupHeader(DatasetGroupId datasetGroupId, uint8_t versionNumber) {
    DatasetsGroupHeader* datasetsGroupHeader = (DatasetsGroupHeader*)malloc(sizeof(DatasetsGroupHeader));
    datasetsGroupHeader->datasetGroupId = datasetGroupId;
    datasetsGroupHeader->versionNumber = versionNumber;
    datasetsGroupHeader->datasetsId = initVectorUint64();
    datasetsGroupHeader->hasSeek = true;
    datasetsGroupHeader->seekPosition = 0;
    return datasetsGroupHeader;
}

void freeDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader) {
    freeVectorUint64(datasetsGroupHeader->datasetsId);
    free(datasetsGroupHeader);
}

void addDatasetId(DatasetsGroupHeader* datasetsGroupHeader, DatasetId datasetId) {
    pushBackVectorUint64(datasetsGroupHeader->datasetsId, datasetId);
}

bool writeDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader, FILE* outputFile) {
    uint64_t datasetsGroupHeaderSize = getSizeDatasetsGroupHeader(datasetsGroupHeader);

    bool boxHeaderSuccessfulWrite = writeBoxHeader(outputFile, datasetsGroupHeaderName, datasetsGroupHeaderSize);
    if (!boxHeaderSuccessfulWrite) {
        fprintf(stderr, "Dataset group header error writing type and size.\n");
        return false;
    }
    return writeDatasetsGroupHeaderContent(datasetsGroupHeader, outputFile);
}

bool writeDatasetsGroupHeaderContent(DatasetsGroupHeader* datasetsGroupHeader, FILE* outputFile) {
    bool datasetGroupIdSuccessfulWrite = utils_write(datasetsGroupHeader->datasetGroupId, outputFile);
    size_t versionNumberWrittenSize = fwrite(&datasetsGroupHeader->versionNumber, 1, 1, outputFile);
    if (!datasetGroupIdSuccessfulWrite || versionNumberWrittenSize != 1) {
        fprintf(stderr, "Dataset group header could not write dataset group id or version number.\n");
        return false;
    }
    for (size_t i = 0; i < getSizeVectorUint64(datasetsGroupHeader->datasetsId); i++) {
        DatasetId datasetId = (DatasetId)getValueVectorUint64(datasetsGroupHeader->datasetsId, i);
        bool datasetIdSuccessfulWrite = writeBigEndian16ToFile(datasetId, outputFile);
        if (!datasetIdSuccessfulWrite) {
            fprintf(stderr, "Dataset group header could not write dataset id.\n");
            return false;
        }
    }
    return true;
}

DatasetsGroupHeader* parseDatasetsGroupHeader(uint64_t boxContentSize, FILE* inputFile) {
    if (sizeof(((DatasetsGroupHeader*)0)->datasetGroupId) + sizeof(((DatasetsGroupHeader*)0)->versionNumber) >
        boxContentSize) {
        fprintf(stderr, "Dataset group header content size is lower than minimum.\n");
        return NULL;
    }
    DatasetGroupId datasetGroupId;
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position\n");
    }

    size_t readDatasetGroupIdSize = fread(&datasetGroupId, sizeof(DatasetGroupId), 1, inputFile);
    uint8_t versionNumber;
    size_t readVersionNumberSize = fread(&versionNumber, sizeof(versionNumber), 1, inputFile);
    if (readDatasetGroupIdSize != 1 || readVersionNumberSize != 1) {
        fprintf(stderr, "Dataset group header could not read dataset group id or version number.\n");
        return NULL;
    }
    uint64_t remainingBoxContentSize = boxContentSize - sizeof(DatasetGroupId) - sizeof(versionNumber);
    if (remainingBoxContentSize % sizeof(DatasetId) != 0) {
        fprintf(stderr, "Dataset group header size inconsistent.\n");
        return NULL;
    }
    DatasetsGroupHeader* datasetsGroupHeader = initDatasetsGroupHeader(datasetGroupId, versionNumber);
    datasetsGroupHeader->hasSeek = true;
    datasetsGroupHeader->seekPosition = (size_t)seekPosition;
    while (remainingBoxContentSize > 0) {
        DatasetId datasetIdBigEndian;
        size_t readDatasetIdSize = fread(&datasetIdBigEndian, sizeof(DatasetId), 1, inputFile);
        if (readDatasetIdSize != 1) {
            fprintf(stderr, "Dataset id could not be read in Dataset group header.\n");
            freeDatasetsGroupHeader(datasetsGroupHeader);
            return NULL;
        }
        remainingBoxContentSize -= sizeof(DatasetId);
    }
    return datasetsGroupHeader;
}

uint64_t getSizeContentDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader) {
    return sizeof(datasetsGroupHeader->datasetGroupId) + sizeof(datasetsGroupHeader->versionNumber) +
           (sizeof(DatasetId) * getSizeVectorUint64(datasetsGroupHeader->datasetsId));
}

uint64_t getSizeDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader) {
    return BOX_HEADER_SIZE + getSizeContentDatasetsGroupHeader(datasetsGroupHeader);
}

size_t getDatasetGroupHeaderSeekPosition(DatasetsGroupHeader* datasetsGroupHeader) {
    return datasetsGroupHeader->seekPosition;
}
