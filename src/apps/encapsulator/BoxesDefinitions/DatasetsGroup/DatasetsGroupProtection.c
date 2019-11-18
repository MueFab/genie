//
// Created by gencom on 27/07/17.
//

#include <stdio.h>
#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"

DatasetsGroupProtection* initDatasetsGroupProtection() {
    DatasetsGroupProtection* datasetsGroupProtection =
        (DatasetsGroupProtection*)malloc(sizeof(DatasetsGroupProtection));
    datasetsGroupProtection->protection = NULL;
    datasetsGroupProtection->hasSeek = false;
    datasetsGroupProtection->seekPosition = 0;
    return datasetsGroupProtection;
}

void freeDatasetsGroupProtection(DatasetsGroupProtection* datasetsGroupProtection) {
    freeByteArray(datasetsGroupProtection->protection);
    free(datasetsGroupProtection);
}

bool defineDatasetsGroupProtectionContent(DatasetsGroupProtection* datasetsGroupProtection, char* filename) {
    FILE* protectionInput = fopen(filename, "rb");
    if (protectionInput != NULL) {
        int errorSeekingEnd = fseek(protectionInput, 0, SEEK_END);
        long int size = ftell(protectionInput);
        int errorSeekingStart = fseek(protectionInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining protection size.\n");
            return false;
        }
        datasetsGroupProtection->protection = initByteArrayWithSize((uint64_t)size);
        Byte* buffer = (Byte*)malloc((uint64_t)size);
        if (buffer == NULL) {
            fprintf(stderr, "Protection memory buffer could not be created.\n");
            return false;
        }
        size_t protectionReadSize = fread(buffer, 1, (uint64_t)size, protectionInput);
        if (protectionReadSize != (uint64_t)size) {
            fprintf(stderr, "Protection input could not be read.\n");
            free(buffer);
            return false;
        }
        copyBytesSource(datasetsGroupProtection->protection, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Protection input could not be opened.\n");
    return false;
}

bool writeDatasetsGroupProtectionContent(DatasetsGroupProtection* datasetsGroupProtection, FILE* outputFile) {
    if (datasetsGroupProtection->protection != NULL) {
        return writeByteArray(datasetsGroupProtection->protection, outputFile);
    }
    fprintf(stderr, "Protection data is not set.\n");
    return false;
}

bool writeDatasetsGroupProtection(DatasetsGroupProtection* datasetsGroupProtection, FILE* outputFile) {
    bool typeSuccessfulWrite = fwrite(datasetsGroupProtectionName, sizeof(char), 4, outputFile) == 4;
    uint64_t datasetsGroupProtectionSize = getDatasetsGroupProtectionSize(datasetsGroupProtection);
    uint64_t datasetsGroupProtectionSizeBigEndian = nativeToBigEndian64(datasetsGroupProtectionSize);
    bool sizeSuccessfulWrite = fwrite(&datasetsGroupProtectionSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error writing type or size of datasets group protection.\n");
        return false;
    }
    return writeDatasetsGroupProtectionContent(datasetsGroupProtection, outputFile);
}

uint64_t getDatasetsGroupProtectionContentSize(DatasetsGroupProtection* datasetsGroupProtection) {
    if (datasetsGroupProtection->protection != NULL) {
        return getSizeByteArray(datasetsGroupProtection->protection);
    }
    return 0;
}

uint64_t getDatasetsGroupProtectionSize(DatasetsGroupProtection* datasetsGroupProtection) {
    return BOX_HEADER_SIZE + getDatasetsGroupProtectionContentSize(datasetsGroupProtection);
}

DatasetsGroupProtection* parseDatasetsGroupProtection(uint64_t boxContentSize, FILE* inputFile) {
    Byte* protectionBuffer = (Byte*)malloc(boxContentSize * sizeof(Byte));
    if (protectionBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for protection buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        free(protectionBuffer);
        return NULL;
    }
    size_t protectionBufferReadSize = fread(protectionBuffer, sizeof(Byte), boxContentSize, inputFile);
    if (protectionBufferReadSize != boxContentSize) {
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(protectionBuffer);
        return NULL;
    }
    DatasetsGroupProtection* datasetsGroupProtection = initDatasetsGroupProtection();
    datasetsGroupProtection->hasSeek = true;
    datasetsGroupProtection->seekPosition = (size_t)seekPosition;
    copyContentDatasetsGroupProtection(datasetsGroupProtection, (char*)protectionBuffer, boxContentSize);
    free(protectionBuffer);
    return datasetsGroupProtection;
}

bool copyContentDatasetsGroupProtection(DatasetsGroupProtection* datasetsGroupProtection, char* content,
                                        uint64_t contentSize) {
    datasetsGroupProtection->protection = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(datasetsGroupProtection->protection, (Byte*)content, contentSize);
}

size_t getDatasetGroupProtectionSeekPosition(DatasetsGroupProtection* datasetsGroupProtection) {
    return datasetsGroupProtection->seekPosition;
}