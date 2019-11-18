//
// Created by bscuser on 26/02/18.
//
#include "../Boxes.h"
#include "../utils.h"
#include "DataUnits.h"

void freeDataUnitBlockHeader(DataUnitBlockHeader *dataUnitBlockHeader) { free(dataUnitBlockHeader); }

bool writeDataUnitBlockHeader(FILE *outputFile, DataUnitBlockHeader *dataUnitBlockHeader) {
    if (dataUnitBlockHeader == NULL) {
        return false;
    }
    uint8_t descriptorIdReservedPadding = dataUnitBlockHeader->descriptorId;
    bool descriptorIdReservedPaddingSuccessfulWrite = utils_write(descriptorIdReservedPadding, outputFile);
    bool blockSizeSuccessfulWrite = writeBigEndian32ToFile(dataUnitBlockHeader->blockSize, outputFile);

    if (!descriptorIdReservedPaddingSuccessfulWrite || !blockSizeSuccessfulWrite) {
        return false;
    }
    return true;
}

DataUnitBlockHeader *initDataUnitBlockHeader(uint8_t descriptorId, uint32_t blockSize) {
    DataUnitBlockHeader *blockHeader = (DataUnitBlockHeader *)calloc(1, sizeof(DataUnitBlockHeader));
    if (blockHeader == NULL) {
        return NULL;
    }
    blockHeader->descriptorId = descriptorId;
    blockHeader->blockSize = blockSize;
    return blockHeader;
}