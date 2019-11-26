//
// Created by gencom on 26/07/17.
//

#include <stdlib.h>
#include "../../Boxes.h"
#include "../../utils.h"

StreamProtection* initStreamProtection(StreamContainer* streamContainer) {
    StreamProtection* streamProtection = (StreamProtection*)malloc(sizeof(StreamProtection));
    streamProtection->protection = NULL;
    streamProtection->streamContainer = streamContainer;
    streamProtection->hasSeek = false;
    streamProtection->seekPosition = 0;
    return streamProtection;
}

void freeStreamProtection(StreamProtection* streamProtection) {
    if (streamProtection != NULL) {
        freeByteArray(streamProtection->protection);
        free(streamProtection);
    }
}

bool defineContentStreamProtection(StreamProtection* streamProtection, char* filename) {
    FILE* protectionInput = fopen(filename, "rb");
    if (protectionInput != NULL) {
        int errorSeekingEnd = fseek(protectionInput, 0, SEEK_END);
        long int size = ftell(protectionInput);
        int errorSeekingStart = fseek(protectionInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining protection size.\n");
            return false;
        }
        streamProtection->protection = initByteArrayWithSize((uint64_t)size);
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
        copyBytesSource(streamProtection->protection, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Protection input could not be opened.\n");
    return false;
}

bool writeContentStreamProtection(StreamProtection* streamProtection, FILE* outputFile) {
    if (streamProtection->protection != NULL) {
        return writeByteArray(streamProtection->protection, outputFile);
    }
    fprintf(stderr, "Stream protection data not set.\n");
    return false;
}

bool writeStreamProtection(StreamProtection* streamProtection, FILE* outputFile) {
    bool typeWriteSuccessful = fwrite(streamProtectionName, sizeof(char), 4, outputFile) == 4;
    uint64_t streamProtectionSize = getSizeStreamProtection(streamProtection);
    uint64_t streamProtectionSizeBigEndian = nativeToBigEndian64(streamProtectionSize);
    bool sizeWriteSuccessful = fwrite(&streamProtectionSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeWriteSuccessful || !sizeWriteSuccessful) {
    }
    return writeContentStreamProtection(streamProtection, outputFile);
}

uint64_t getSizeContentStreamProtection(StreamProtection* streamProtection) {
    if (streamProtection->protection != NULL) {
        return getSizeByteArray(streamProtection->protection);
    }
    return 0;
}

uint64_t getSizeStreamProtection(StreamProtection* streamProtection) {
    return BOX_HEADER_SIZE + getSizeContentStreamProtection(streamProtection);
}

bool copyContentStreamProtection(StreamProtection* streamProtection, char* content, uint64_t contentSize) {
    streamProtection->protection = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(streamProtection->protection, (Byte*)content, contentSize);
}

StreamProtection* parseStreamProtection(StreamContainer* streamContainer, uint64_t boxContentSize, FILE* inputFile) {
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        return NULL;
    }
    Byte* protectionBuffer = (Byte*)malloc(boxContentSize * sizeof(Byte));
    if (protectionBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for protection buffer.\n");
        return NULL;
    }
    size_t protectionBufferReadSize = fread(protectionBuffer, sizeof(Byte), boxContentSize, inputFile);
    if (protectionBufferReadSize != boxContentSize) {
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(protectionBuffer);
        return NULL;
    }
    StreamProtection* streamProtection = initStreamProtection(streamContainer);
    streamProtection->hasSeek = true;
    streamProtection->seekPosition = (size_t)seekPosition;
    copyContentStreamProtection(streamProtection, (char*)protectionBuffer, boxContentSize);
    free(protectionBuffer);
    return streamProtection;
}

size_t getStreamProtectionSeekPosition(StreamProtection* streamProtection) { return streamProtection->seekPosition; }
