//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"

DatasetMetadata* initDatasetMetadata() {
    DatasetMetadata* datasetMetadata = (DatasetMetadata*)malloc(sizeof(DatasetMetadata));
    datasetMetadata->metadata = NULL;
    datasetMetadata->hasSeek = false;
    datasetMetadata->seekPosition = 0;
    return datasetMetadata;
}
void freeDatasetMetadata(DatasetMetadata* datasetMetadata) {
    freeByteArray(datasetMetadata->metadata);
    free(datasetMetadata);
}
bool defineContentDatasetMetadata(DatasetMetadata* datasetMetadata, char* filename) {
    FILE* metadataInput = fopen(filename, "rb");
    if (metadataInput != NULL) {
        int errorSeekingEnd = fseek(metadataInput, 0, SEEK_END);
        long int size = ftell(metadataInput);
        int errorSeekingStart = fseek(metadataInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining protection size.\n");
            return false;
        }
        datasetMetadata->metadata = initByteArrayWithSize((uint64_t)size);
        Byte* buffer = (Byte*)malloc((uint64_t)size);
        if (buffer == NULL) {
            fprintf(stderr, "Metadata memory buffer could not be created.\n");
            return false;
        }
        size_t metadataReadSize = fread(buffer, 1, (uint64_t)size, metadataInput);
        if (metadataReadSize != (uint64_t)size) {
            fprintf(stderr, "Dataset metadata input could not be read.\n");
            free(buffer);
            return false;
        }
        copyBytesSource(datasetMetadata->metadata, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Metadata input could not be opened.\n");
    return false;
}

bool writeDatasetMetadata(DatasetMetadata* datasetMetadata, FILE* outputFile) {
    bool typeSuccessfulWrite = fwrite(datasetMetadataName, sizeof(char), 4, outputFile) == 4;
    uint64_t datasetMetadataSize = getSizeDatasetMetadata(datasetMetadata);
    uint64_t datasetMetadataSizeBigEndian = nativeToBigEndian64(datasetMetadataSize);
    bool sizeSuccessfulWrite = fwrite(&datasetMetadataSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error writing type and size of Dataset Metadata.\n");
        return false;
    }
    return writeContentDatasetMetadata(datasetMetadata, outputFile);
}

bool writeContentDatasetMetadata(DatasetMetadata* datasetMetadata, FILE* outputFile) {
    if (datasetMetadata->metadata != NULL) {
        return writeByteArray(datasetMetadata->metadata, outputFile);
    }
    fprintf(stderr, "Metadata data not set.\n");
    return false;
}

DatasetMetadata* parseDatasetMetadata(uint64_t boxContentSize, FILE* inputFile) {
    Byte* metadataBuffer = (Byte*)malloc(boxContentSize * sizeof(Byte));
    if (metadataBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for metadata buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        free(metadataBuffer);
        return NULL;
    }

    size_t metadataBufferReadSize = fread(metadataBuffer, sizeof(Byte), boxContentSize, inputFile);
    if (metadataBufferReadSize != boxContentSize) {
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(metadataBuffer);
        return NULL;
    }
    DatasetMetadata* datasetMetadata = initDatasetMetadata();
    datasetMetadata->hasSeek = true;
    datasetMetadata->seekPosition = (size_t)seekPosition;
    copyContentDatasetMetadata(datasetMetadata, (char*)metadataBuffer, boxContentSize);
    free(metadataBuffer);
    return datasetMetadata;
}

uint64_t getSizeContentDatasetMetadata(DatasetMetadata* datasetMetadata) {
    if (datasetMetadata->metadata != NULL) {
        return getSizeByteArray(datasetMetadata->metadata);
    }
    return 0;
}

uint64_t getSizeDatasetMetadata(DatasetMetadata* datasetMetadata) {
    return getSizeContentDatasetMetadata(datasetMetadata) + 12;
}

bool copyContentDatasetMetadata(DatasetMetadata* datasetMetadata, char* content, uint64_t contentSize) {
    datasetMetadata->metadata = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(datasetMetadata->metadata, (Byte*)content, contentSize);
}

size_t getDatasetMetadataSeekPosition(DatasetMetadata* datasetMetadata) { return datasetMetadata->seekPosition; }