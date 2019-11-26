//
// Created by bscuser on 7/02/18.
//

#include "../../Boxes.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../utils.h"

BlockHeader* initBlockHeader(DatasetContainer* datasetContainer, uint8_t descriptorId, uint32_t payloadSize) {
    BlockHeader* blockHeader = (BlockHeader*)malloc(sizeof(BlockHeader));
    if (blockHeader == NULL) {
        fprintf(stderr, "Block Header could not be allocated.\n");
        return NULL;
    }
    blockHeader->datasetContainer = datasetContainer;
    blockHeader->descriptorId = descriptorId;
    blockHeader->payloadSize = payloadSize;

    return blockHeader;
}

uint8_t getBlocksDescriptorId(BlockHeader* blockHeader) { return blockHeader->descriptorId; }

uint32_t getPayloadSize(BlockHeader* blockHeader) { return blockHeader->payloadSize; }

BlockHeader* parseBlockHeader(DatasetContainer* datasetContainer, FILE* inputFile) {
    uint8_t descritorIdBuffer;
    uint32_t payloadSizeBuffer;

    bool descritorIdAndPaddingFlagSuccessfulRead = utils_read(&descritorIdBuffer, inputFile);
    if (!descritorIdAndPaddingFlagSuccessfulRead) {
        fprintf(stderr, "Error reading descriptor Id or padding flag.\n");
        return NULL;
    }
    bool payloadSizeSuccessfulRead = readBigEndian32FromFile(&payloadSizeBuffer, inputFile);
    if (!payloadSizeSuccessfulRead) {
        fprintf(stderr, "Error reading reserved value.\n");
        return NULL;
    }
    return initBlockHeader(datasetContainer, descritorIdBuffer, payloadSizeBuffer);
}

bool writeBlockHeader(BlockHeader* blockHeader, FILE* outputFile) {
    bool descriptorIdSuccessfulWrite = utils_write(blockHeader->descriptorId, outputFile);
    bool payloadSizeSuccessfulWrite = writeBigEndian32ToFile(blockHeader->payloadSize, outputFile);
    if (!descriptorIdSuccessfulWrite || !payloadSizeSuccessfulWrite) {
        fprintf(stderr, "Error writing descritor id, padding flag, reserved value or paylaod size.\n");
        return false;
    }
    return true;
}

void freeBlockHeader(BlockHeader* blockHeader) { free(blockHeader); }

uint64_t getBlockHeaderSize(BlockHeader* blockHeader) {
    uint64_t blockHeaderSize = 0;
    blockHeaderSize += 1;
    blockHeaderSize += 4;
    return blockHeaderSize;
}