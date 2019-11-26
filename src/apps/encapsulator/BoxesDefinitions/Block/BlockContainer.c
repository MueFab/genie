//
// Created by bscuser on 7/02/18.
//
#include "../../Boxes.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../utils.h"

void freeBlock(Block* block) {
    if (block->blockHeader != NULL) {
        freeBlockHeader(block->blockHeader);
    }
    if (block->payload != NULL) {
        freeFromFile(block->payload);
    }
    free(block);
}

Block* initBlock(DatasetContainer* datasetContainer, FromFile* fromFile) {
    Block* block = (Block*)malloc(sizeof(Block));
    if (block == NULL) {
        fprintf(stderr, "Block could not be allocated.\n");
        return NULL;
    }
    block->blockHeader = NULL;
    block->payload = fromFile;
    block->datasetContainer = datasetContainer;

    return block;
}

Block* initBlockWithHeader(uint8_t descriptorId, uint32_t payloadSize, FromFile* payload) {
    Block* block = (Block*)malloc(sizeof(Block));
    if (block == NULL) {
        fprintf(stderr, "Block could not be allocated.\n");
        return NULL;
    }
    block->blockHeader = (BlockHeader*)malloc(sizeof(BlockHeader));
    if (block->blockHeader == NULL) {
        fprintf(stderr, "Block header could not be allocated.\n");
        free(block);
        return NULL;
    }
    block->blockHeader->payloadSize = payloadSize;
    block->blockHeader->descriptorId = descriptorId;
    block->blockHeader->datasetContainer = NULL;

    block->payload = payload;
    block->datasetContainer = NULL;

    return block;
}

void setBlockHeader(Block* block, BlockHeader* blockHeader) { block->blockHeader = blockHeader; }

void setPaddingSize(Block* block, uint32_t paddingSize) { block->padding_size = paddingSize; }

bool writeBlock(Block* block, FILE* outputFile) {
    if (isBlockHeaderFlagSet(getDatasetHeader(block->datasetContainer))) {
        if (block->blockHeader != NULL) {
            if (!writeBlockHeader(block->blockHeader, outputFile)) {
                fprintf(stderr, "error writing block's header.\n");
                return false;
            };
        } else {
            fprintf(stderr, "Missing block header to write.\n");
            return false;
        }
    }
    if (block->payload != NULL) {
        if (!writeFromFile(block->payload, outputFile)) {
            fprintf(stderr, "Block's payload: error writing.\n");
            return false;
        }
    }
    return true;
}

Block* parseBlockContainerAUCmode(DatasetContainer* datasetContainer, FILE* inputFile, char* fileName) {
    BlockHeader* blockHeader = NULL;
    if (isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))) {
        blockHeader = parseBlockHeader(datasetContainer, inputFile);
        if (blockHeader == NULL) {
            fprintf(stderr, "Block cannot be read without block header.\n");
            return NULL;
        };
    } else {
        fprintf(stderr, "ParseBlockContainerAUCmode should only be called in AUC mode.\n");
        return NULL;
    }
    FromFile* blockPayload = initFromFileWithFilenameAndBoundaries(
        fileName, (uint64_t)ftell(inputFile), (uint64_t)(ftell(inputFile) + getPayloadSize(blockHeader)));
    if (fseek(inputFile, getPayloadSize(blockHeader), SEEK_CUR) != 0) {
        fprintf(stderr, "Error seeking position after end of block.\n");
        freeFromFile(blockPayload);
        freeBlockHeader(blockHeader);
        return NULL;
    }

    if (feof(inputFile)) {
        fprintf(stderr, "Error reading block: reached EOF.\n");
        freeFromFile(blockPayload);
        freeBlockHeader(blockHeader);
        return NULL;
    }

    Block* block = initBlock(datasetContainer, blockPayload);
    setBlockHeader(block, blockHeader);
    return block;
}

uint64_t getBlockSize(Block* block) {
    uint64_t blockSize = 0;
    if (block->datasetContainer != NULL) {
        if (isBlockHeaderFlagSet(getDatasetHeader(block->datasetContainer))) {
            if (block->blockHeader != NULL) {
                blockSize += getBlockHeaderSize(block->blockHeader);
            }
        }
    }
    if (block->payload != NULL) {
        blockSize += getFromFileSize(block->payload);
    }

    return blockSize;
}
