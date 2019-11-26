//
// Created by gencom on 4/12/17.
//

#include "../../Boxes.h"
#include "../../utils.h"

AccessUnitProtection* initAccessUnitProtection() {
    AccessUnitProtection* accessUnitProtection = (AccessUnitProtection*)malloc(sizeof(AccessUnitProtection));
    accessUnitProtection->protection = NULL;
    accessUnitProtection->seekPosition = -1;
    return accessUnitProtection;
}

void freeAccessUnitProtection(AccessUnitProtection* accessUnitProtection) {
    freeByteArray(accessUnitProtection->protection);
    free(accessUnitProtection);
}

bool defineContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection, char* filename) {
    FILE* protectionInput = fopen(filename, "rb");
    if (protectionInput != NULL) {
        int errorSeekingEnd = fseek(protectionInput, 0, SEEK_END);
        long int size = ftell(protectionInput);
        int errorSeekingStart = fseek(protectionInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining protection size.\n");
            return false;
        }
        accessUnitProtection->protection = initByteArrayWithSize((uint64_t)size);
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
        copyBytesSource(accessUnitProtection->protection, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Protection input could not be opened.\n");
    return false;
}

bool writeContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection, FILE* outputFile) {
    if (accessUnitProtection->protection != NULL) {
        return writeByteArray(accessUnitProtection->protection, outputFile);
    }
    fprintf(stderr, "Dataset protection does not have data to print.\n");
    return false;
}

bool writeAccessUnitProtection(AccessUnitProtection* accessUnitProtection, FILE* outputFile) {
    bool typeSuccessfulWrite = fwrite(accessUnitProtectionName, sizeof(char), 4, outputFile) == 4;
    uint64_t accessUnitProtectionSize = getSizeAccessUnitProtection(accessUnitProtection);
    uint64_t accessUnitProtectionSizeBigEndian = nativeToBigEndian64(accessUnitProtectionSize);
    bool sizeSuccessfulWrite = fwrite(&accessUnitProtectionSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error printing type and size of dataset protection.\n");
        return false;
    }
    return writeContentAccessUnitProtection(accessUnitProtection, outputFile);
}

uint64_t getSizeContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection) {
    if (accessUnitProtection->protection != NULL) {
        return getSizeByteArray(accessUnitProtection->protection);
    }
    return 0;
}

uint64_t getSizeAccessUnitProtection(AccessUnitProtection* accessUnitProtection) {
    return BOX_HEADER_SIZE + getSizeContentAccessUnitProtection(accessUnitProtection);
}

AccessUnitProtection* parseAccessUnitProtection(uint64_t boxContentSize, FILE* inputFile) {
    Byte* protectionBuffer = (Byte*)malloc(boxContentSize * sizeof(Byte));
    if (protectionBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for protection buffer.\n");
        return NULL;
    }
    long seekPosition = ftell(inputFile);
    size_t protectionBufferReadSize = fread(protectionBuffer, sizeof(Byte), boxContentSize, inputFile);
    if (protectionBufferReadSize != boxContentSize) {
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(protectionBuffer);
        return NULL;
    }
    AccessUnitProtection* accessUnitProtection = initAccessUnitProtection();
    copyContentAccessUnitProtection(accessUnitProtection, (char*)protectionBuffer, boxContentSize);
    free(protectionBuffer);
    accessUnitProtection->seekPosition = seekPosition;
    return accessUnitProtection;
}

bool copyContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection, char* content, uint64_t contentSize) {
    accessUnitProtection->protection = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(accessUnitProtection->protection, (Byte*)content, contentSize);
}

long getAccessUnitProtectionSeekPosition(AccessUnitProtection* accessUnitInformation) {
    return accessUnitInformation->seekPosition;
}