//
// Created by gencom on 26/07/17.
//

#include <memory.h>
#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"

MPEGGFile* initFile() {
    MPEGGFile* file = (MPEGGFile*)malloc(sizeof(MPEGGFile));
    file->fileHeader = NULL;
    file->datasetsGroups = initVector();
    return file;
}

void freeFile(MPEGGFile* file) {
    freeFileHeader(file->fileHeader);
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for (size_t datasetsGroupContainer_i = 0; datasetsGroupContainer_i < datasetsGroupContainer_size;
         datasetsGroupContainer_i++) {
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        if (datasetsGroupContainer != NULL) {
            freeDatasetsGroupContainer(datasetsGroupContainer);
        }
    }
    freeVector(file->datasetsGroups);
    free(file);
}

void setFileHeaderToFile(MPEGGFile* file, FileHeader* fileHeader) { file->fileHeader = fileHeader; }

void addDatasetsGroupToFile(MPEGGFile* file, DatasetsGroupContainer* datasetsGroupContainer) {
    pushBack(file->datasetsGroups, datasetsGroupContainer);
}

bool writeFile(MPEGGFile* file, FILE* outputFile) {
    if (!isFileValid(file)) {
        fprintf(stderr, "File instance is not valid.\n");
        return false;
    }
    if (!writeFileHeader(file->fileHeader, outputFile)) {
        fprintf(stderr, "Error writing file header.\n");
        return false;
    }
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for (size_t datasetsGroupContainer_i = 0; datasetsGroupContainer_i < datasetsGroupContainer_size;
         datasetsGroupContainer_i++) {
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        if (datasetsGroupContainer != NULL) {
            if (!writeDatasetsGroupContainer(datasetsGroupContainer, outputFile)) {
                fprintf(stderr, "Error writing dataset group index:%lu.\n", datasetsGroupContainer_i);
                return false;
            }
        }
    }
    return true;
}

bool generateFileSeekPoints(MPEGGFile* file) {
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for (size_t datasetsGroupContainer_i = 0; datasetsGroupContainer_i < datasetsGroupContainer_size;
         datasetsGroupContainer_i++) {
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        generateDatasetsGroupSeekPoints(datasetsGroupContainer);
    }
    return true;
}

bool isFileValid(MPEGGFile* file) {
    if (file->fileHeader == NULL) return false;
    size_t datasetsGroupContainer_size = getSize(file->datasetsGroups);
    for (size_t datasetsGroupContainer_i = 0; datasetsGroupContainer_i < datasetsGroupContainer_size;
         datasetsGroupContainer_i++) {
        DatasetsGroupContainer* datasetsGroupContainer = getValue(file->datasetsGroups, datasetsGroupContainer_i);
        if (datasetsGroupContainer != NULL) {
            if (!isDatasetsGroupContainerValid(datasetsGroupContainer)) {
                return false;
            }
        }
    }
    return true;
}

MPEGGFile* parseFile(FILE* inputFile, char* filename) {
    char boxType[5];
    uint64_t boxSize;
    bool successfulRead = readBoxHeader(inputFile, boxType, &boxSize);
    if (successfulRead) {
        MPEGGFile* file = (MPEGGFile*)malloc(sizeof(MPEGGFile));

        file->fileHeader = NULL;
        file->datasetsGroups = initVector();

        if (strncmp(boxType, fileHeaderBoxName, 4) == 0) {
            FileHeader* fileHeader = parseFileHeader(boxSize - 12, inputFile);
            setFileHeaderToFile(file, fileHeader);
        } else {
            fprintf(stderr, "Cannot parse: expected file header not found.\n");
            return NULL;
        }
        while (readBoxHeader(inputFile, boxType, &boxSize)) {
            if (strncmp(boxType, "dgcn", 4) == 0) {
                DatasetsGroupContainer* datasetsGroupContainer =
                    parseDatasetsGroupContainer(inputFile, boxSize - 12, filename);
                if (datasetsGroupContainer == NULL) {
                    fprintf(stderr, "Error reading datasets group container.\n");
                    freeFile(file);
                    return NULL;
                }
                addDatasetsGroupToFile(file, datasetsGroupContainer);
            } else {
                fprintf(stderr, "Cannot parse: expected dataset group not found.\n");
                freeFile(file);
                return NULL;
            }
        }
        return file;
    }
    fprintf(stderr, "Cannot read the file.\n");
    return NULL;
}

FileHeader* getFileHeader(MPEGGFile* file) { return file->fileHeader; }

DatasetsGroupContainer* getDatasetGroupContainerByIndex(MPEGGFile* file, size_t index) {
    if (index >= getSize(file->datasetsGroups)) {
        return NULL;
    }
    return getValue(file->datasetsGroups, index);
}

DatasetsGroupContainer* getDatasetGroupContainerById(MPEGGFile* file, DatasetGroupId datasetGroupId) {
    size_t numDatasetGroups = getSize(file->datasetsGroups);
    for (size_t datasetGroup_i = 0; datasetGroup_i < numDatasetGroups; datasetGroup_i++) {
        DatasetsGroupContainer* datasetsGroupContainer = getDatasetGroupContainerByIndex(file, datasetGroup_i);
        if (datasetsGroupContainer == NULL) {
            continue;
        }
        if (datasetsGroupContainer->datasetsGroupHeader->datasetGroupId == datasetGroupId) {
            return datasetsGroupContainer;
        }
    }
    return NULL;
}

unsigned long getDatasetsGroupContainersSize(MPEGGFile* file) { return getSize(file->datasetsGroups); }
