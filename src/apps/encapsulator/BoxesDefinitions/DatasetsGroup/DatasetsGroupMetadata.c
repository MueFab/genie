//
// Created by gencom on 27/07/17.
//

#include <stdio.h>
#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"

DatasetsGroupMetadata* initDatasetsGroupMetadata() {
    DatasetsGroupMetadata* datasetsGroupMetadata = (DatasetsGroupMetadata*)malloc(sizeof(DatasetsGroupMetadata));
    datasetsGroupMetadata->hasSeek = true;
    datasetsGroupMetadata->seekPosition = 0;
    return datasetsGroupMetadata;
}

void freeDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata) {
    freeByteArray(datasetsGroupMetadata->metadata);
    free(datasetsGroupMetadata);
}

bool copyContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, char* content,
                                      uint64_t contentSize) {
    datasetsGroupMetadata->metadata = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(datasetsGroupMetadata->metadata, (Byte*)content, contentSize);
}

bool defineContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, char* filename) {
    FILE* metadataInput = fopen(filename, "rb");
    if (metadataInput != NULL) {
        int errorSeekingEnd = fseek(metadataInput, 0, SEEK_END);
        long int size = ftell(metadataInput);
        int errorSeekingStart = fseek(metadataInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining protection size.\n");
            return false;
        }
        datasetsGroupMetadata->metadata = initByteArrayWithSize((uint64_t)size);
        Byte* buffer = (Byte*)malloc((uint64_t)size);
        if (buffer == NULL) {
            fprintf(stderr, "Dataset group metadata memory buffer could not be created.\n");
            return false;
        }
        size_t metadataReadSize = fread(buffer, 1, (uint64_t)size, metadataInput);
        if (metadataReadSize != (uint64_t)size) {
            fprintf(stderr, "Dataset group metadata input could not be read.\n");
            free(buffer);
            return false;
        }
        copyBytesSource(datasetsGroupMetadata->metadata, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Dataset group metadata input could not be opened.\n");
    return false;
}

bool writeDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, FILE* outputFile) {
    bool typeWriteSuccessful = fwrite(datasetsGroupMetadataName, sizeof(char), 4, outputFile) == 4;
    uint64_t datasetsGroupMetadataSize = getSizeDatasetsGroupMetadata(datasetsGroupMetadata);
    uint64_t datasetsGroupMetadataSizeBigEndian = nativeToBigEndian64(datasetsGroupMetadataSize);
    bool sizeWriteSuccessful = fwrite(&datasetsGroupMetadataSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeWriteSuccessful || !sizeWriteSuccessful) {
        return false;
    }
    return writeContentDatasetsGroupMetadata(datasetsGroupMetadata, outputFile);
}

bool writeContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, FILE* outputFile) {
    if (datasetsGroupMetadata->metadata != NULL) {
        return writeByteArray(datasetsGroupMetadata->metadata, outputFile);
    }
    fprintf(stderr, "Dataset group metadata input data not set.\n");
    return false;
}

DatasetsGroupMetadata* parseDatasetsGroupMetadata(uint64_t boxContentSize, FILE* inputFile) {
    Byte* metadataBuffer = (Byte*)malloc((boxContentSize + 1) * sizeof(Byte));
    if (metadataBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for metadata buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not read file position.\n");
        free(metadataBuffer);
        return NULL;
    }
    size_t metadataBufferReadSize = fread(metadataBuffer, sizeof(Byte), boxContentSize, inputFile);
    if (metadataBufferReadSize != boxContentSize) {
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(metadataBuffer);
        return NULL;
    }
    metadataBuffer[boxContentSize] = '\0';
    DatasetsGroupMetadata* datasetsGroupMetadata = initDatasetsGroupMetadata();
    datasetsGroupMetadata->hasSeek = true;
    datasetsGroupMetadata->seekPosition = (size_t)seekPosition;
    copyContentDatasetsGroupMetadata(datasetsGroupMetadata, (char*)metadataBuffer, boxContentSize);
    free(metadataBuffer);
    return datasetsGroupMetadata;
}

uint64_t getSizeDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata) {
    return BOX_HEADER_SIZE + getSizeContentDatasetsGroupMetadata(datasetsGroupMetadata);
}

uint64_t getSizeContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata) {
    return getSizeByteArray(datasetsGroupMetadata->metadata);
}

size_t getDatasetGroupMetadataSeekPosition(DatasetsGroupMetadata* datasetsGroupMetadata) {
    return datasetsGroupMetadata->seekPosition;
}
