//
// Created by gencom on 4/12/17.
//

#include "../../Boxes.h"
#include "../../utils.h"

AccessUnitInformation* initAccessUnitInformation() {
    AccessUnitInformation* accessUnitInformation = (AccessUnitInformation*)malloc(sizeof(AccessUnitInformation));
    accessUnitInformation->information = NULL;
    accessUnitInformation->seekPosition = -1;
    return accessUnitInformation;
}

void freeAccessUnitInformation(AccessUnitInformation* accessUnitInformation) {
    freeByteArray(accessUnitInformation->information);
    free(accessUnitInformation);
}

bool defineContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation, char* filename) {
    FILE* informationInput = fopen(filename, "rb");
    if (informationInput != NULL) {
        int errorSeekingEnd = fseek(informationInput, 0, SEEK_END);
        long int size = ftell(informationInput);
        int errorSeekingStart = fseek(informationInput, 0, SEEK_SET);
        if (errorSeekingEnd != 0 || size <= 0L || errorSeekingStart != 0) {
            fprintf(stderr, "Error while determining information size.\n");
            return false;
        }
        accessUnitInformation->information = initByteArrayWithSize((uint64_t)size);
        Byte* buffer = (Byte*)malloc((uint64_t)size);
        if (buffer == NULL) {
            fprintf(stderr, "Information memory buffer could not be created.\n");
            return false;
        }
        size_t informationReadSize = fread(buffer, 1, (uint64_t)size, informationInput);
        if (informationReadSize != (uint64_t)size) {
            fprintf(stderr, "Information input could not be read.\n");
            free(buffer);
            return false;
        }
        copyBytesSource(accessUnitInformation->information, buffer, (uint64_t)size);
        free(buffer);
        return true;
    }
    fprintf(stderr, "Information input could not be opened.\n");
    return false;
}

bool writeContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation, FILE* outputFile) {
    if (accessUnitInformation->information != NULL) {
        return writeByteArray(accessUnitInformation->information, outputFile);
    }
    fprintf(stderr, "Dataset information does not have data to print.\n");
    return false;
}

bool writeAccessUnitInformation(FILE* outputFile, AccessUnitInformation* accessUnitInformation) {
    bool typeSuccessfulWrite = fwrite(accessUnitInformationName, sizeof(char), 4, outputFile) == 4;
    uint64_t accessUnitInformationSize = getSizeAccessUnitInformation(accessUnitInformation);
    uint64_t accessUnitInformationSizeBigEndian = nativeToBigEndian64(accessUnitInformationSize);
    bool sizeSuccessfulWrite = fwrite(&accessUnitInformationSizeBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error printing type and size of dataset information.\n");
        return false;
    }
    return writeContentAccessUnitInformation(accessUnitInformation, outputFile);
}

uint64_t getSizeContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation) {
    if (accessUnitInformation->information != NULL) {
        return getSizeByteArray(accessUnitInformation->information);
    }
    return 0;
}

uint64_t getSizeAccessUnitInformation(AccessUnitInformation* accessUnitInformation) {
    return BOX_HEADER_SIZE + getSizeContentAccessUnitInformation(accessUnitInformation);
}

AccessUnitInformation* parseAccessUnitInformation(uint64_t boxContentSize, FILE* inputFile) {
    long seekPosition = ftell(inputFile);
    Byte* informationBuffer = (Byte*)malloc(boxContentSize * sizeof(Byte));
    if (informationBuffer == NULL) {
        fprintf(stderr, "Error while reserving memory for information buffer.\n");
        return NULL;
    }
    size_t informationBufferReadSize = fread(informationBuffer, sizeof(Byte), boxContentSize, inputFile);
    if (informationBufferReadSize != boxContentSize) {
        fprintf(stderr, "Could not read requested number of bytes.\n");
        free(informationBuffer);
        return NULL;
    }
    AccessUnitInformation* accessUnitInformation = initAccessUnitInformation();
    copyContentAccessUnitInformation(accessUnitInformation, (char*)informationBuffer, boxContentSize);
    free(informationBuffer);
    accessUnitInformation->seekPosition = seekPosition;
    return accessUnitInformation;
}

bool copyContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation, char* content,
                                      uint64_t contentSize) {
    accessUnitInformation->information = initByteArrayWithSize((uint64_t)contentSize);
    return copyBytesSource(accessUnitInformation->information, (Byte*)content, contentSize);
}

long getAccessUnitInformationSeekPosition(AccessUnitInformation* accessUnitInformation) {
    return accessUnitInformation->seekPosition;
}