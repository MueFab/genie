//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"

DatasetProtection* initDatasetProtection() {
    DatasetProtection* datasetProtection = (DatasetProtection*)malloc(sizeof(DatasetProtection));
    datasetProtection->protection = NULL;
    datasetProtection->hasSeek = false;
    datasetProtection->seekPosition = 0;
    return datasetProtection;
}

void freeDatasetProtection(DatasetProtection* datasetProtection) {
    freeByteArray(datasetProtection->protection);
    free(datasetProtection);
}

bool defineContentDatasetProtection(DatasetProtection* datasetProtection, char* filename) {
    FILE* protectionInput = fopen(filename, "rb");
    if (protectionInput != NULL) {
        int errorSeekingEnd = fseek(protectionInput, 0, SEEK_END);
        long int size = ftell(protectionInput);
        int errorSeekingStart = fseek(protectionInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining protection size.\n");
            return false;
        }
        datasetProtection->protection = initByteArrayWithSize((uint64_t)size);
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
        copyBytesSource(datasetProtection->protection, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Protection input could not be opened.\n");
    return false;
}

bool writeContentDatasetProtection(DatasetProtection* datasetProtection, FILE* outputFile) {
    if (datasetProtection->protection != NULL) {
        return writeByteArray(datasetProtection->protection, outputFile);
    }
    fprintf(stderr, "Dataset protection does not have data to print.\n");
    return false;
}

bool writeDatasetProtection(DatasetProtection* datasetProtection, FILE* outputFile) {
    bool typeSuccessfulWrite = fwrite(datasetProtectionName, sizeof(char), 4, outputFile) == 4;
    uint64_t datasetProtectionSize = getSizeDatasetProtection(datasetProtection);
    uint64_t datasetProtectionSizeBigEndian = nativeToBigEndian64(datasetProtectionSize);
    bool sizeSuccessfulWrite = fwrite(&datasetProtectionSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error printing type and size of dataset protection.\n");
        return false;
    }
    return writeContentDatasetProtection(datasetProtection, outputFile);
}

uint64_t getSizeContentDatasetProtection(DatasetProtection* datasetProtection) {
    if (datasetProtection->protection != NULL) {
        return getSizeByteArray(datasetProtection->protection);
    }
    return 0;
}

uint64_t getSizeDatasetProtection(DatasetProtection* datasetProtection) {
    return BOX_HEADER_SIZE + getSizeContentDatasetProtection(datasetProtection);
}

DatasetProtection* parseDatasetProtection(uint64_t boxContentSize, FILE* inputFile) {
    Byte* protectionBuffer = (Byte*)malloc(boxContentSize * sizeof(Byte));
    if (protectionBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for protection buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    if (ftell(inputFile) == -1) {
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
    DatasetProtection* datasetProtection = initDatasetProtection();
    datasetProtection->hasSeek = true;
    datasetProtection->seekPosition = (size_t)seekPosition;
    copyContentDatasetProtection(datasetProtection, (char*)protectionBuffer, boxContentSize);
    free(protectionBuffer);
    return datasetProtection;
}

bool copyContentDatasetProtection(DatasetProtection* datasetProtection, char* content, uint64_t contentSize) {
    datasetProtection->protection = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(datasetProtection->protection, (Byte*)content, contentSize);
}

size_t getDatasetProtectionSeekPosition(DatasetProtection* datasetProtection) {
    return datasetProtection->seekPosition;
}
