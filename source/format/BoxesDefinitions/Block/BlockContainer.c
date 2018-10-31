//
// Created by bscuser on 7/02/18.
//
#include <Boxes.h>
#include <utils.h>
#include "Boxes.h"

void freeBlock(Block* block){
    if(block->blockHeader != NULL){
        freeBlockHeader(block->blockHeader);
    }
    if(block->payload != NULL){
        freeFromFile(block->payload);
    }
    free(block);
}

Block* initBlock(
    DatasetContainer* datasetContainer,
    FromFile* fromFile
){
    Block* block = (Block*)malloc(sizeof(Block));
    if(block == NULL){
        fprintf(stderr,"Block could not be allocated.\n");
        return NULL;
    }
    block->blockHeader = NULL;
    block->payload = fromFile;
    block->datasetContainer = datasetContainer;

    return block;
}

void setBlockHeader(Block* block, BlockHeader* blockHeader){
    block->blockHeader = blockHeader;
}

void setPaddingSize(Block* block, uint32_t paddingSize){
    block->padding_size = paddingSize;
}

bool writeBlock(Block* block, FILE* outputFile){
    if(isBlockHeaderFlagSet(getDatasetHeader(block->datasetContainer))){
        if(block->blockHeader != NULL) {
            if(!writeBlockHeader(block->blockHeader, outputFile)){
                fprintf(stderr,"error writing block's header.\n");
                return false;
            };
        }else{
            fprintf(stderr,"Missing block header to write.\n");
            return false;
        }
    }
    if(block->payload != NULL) {
        if (!writeFromFile(block->payload, outputFile)) {
            fprintf(stderr, "Block's payload: error writing.\n");
            return false;
        }
    }
    if(block->blockHeader != NULL){
        if(isPaddingFlagSet(block->blockHeader)){
            bool paddingSizeSuccessfulWrite = writeBigEndian32ToFile(block->padding_size, outputFile);
            if(!paddingSizeSuccessfulWrite){
                fprintf(stderr,"Padding size could not be writing.\n");
                return false;
            }
            bool paddingBytesSuccessfulWrite = true;
            for(uint32_t i=0; i<block->padding_size; i++){
                bool paddingByteSuccessfulWrite = write(0,outputFile);
                if (!paddingByteSuccessfulWrite){
                    paddingBytesSuccessfulWrite = false;
                    break;
                }
            }
            if(!paddingBytesSuccessfulWrite){
                fprintf(stderr,"Error writing block's padding.\n");
                return false;
            }
        }
    }
    return true;
}

Block* parseBlockContainerAUCmode(DatasetContainer *datasetContainer, FILE *inputFile, char *fileName){
    BlockHeader* blockHeader = NULL;
    if(isBlockHeaderFlagSet(getDatasetHeader(datasetContainer))){
        blockHeader = parseBlockHeader(datasetContainer, inputFile);
        if(blockHeader == NULL){
            fprintf(stderr,"Block cannot be read without block header.\n");
            return NULL;
        };
    }else{
        fprintf(stderr,"ParseBlockContainerAUCmode should only be called in AUC mode.\n");
        return NULL;
    }
    FromFile* blockPayload =
            initFromFileWithFilenameAndBoundaries(
                    fileName,
                    ftell(inputFile),
                    ftell(inputFile)+getPayloadSize(blockHeader)
            );
    fseek(inputFile, getPayloadSize(blockHeader), SEEK_CUR);

    uint32_t padding_size = 0;
    if(isPaddingFlagSet(blockHeader)){
        bool paddingSizeSuccessfulRead = readBigEndian32FromFile(&padding_size, inputFile);
        if(!paddingSizeSuccessfulRead){
            fprintf(stderr,"Padding size could not be read.\n");
            return NULL;
        }
        fseek(inputFile, padding_size, SEEK_CUR);
    }

    Block* block = initBlock(
            datasetContainer,
            blockPayload
    );
    setBlockHeader(block, blockHeader);
    return block;
}

uint64_t getBlockSize(Block* block){
    uint64_t blockSize = 0;
    if(isBlockHeaderFlagSet(getDatasetHeader(block->datasetContainer))){
        if(block->blockHeader != NULL) {
            blockSize += getBlockHeaderSize(block->blockHeader);
        }
    }
    if(block->payload != NULL) {
        blockSize += getFromFileSize(block->payload);
    }

    if(block->blockHeader != NULL){
        if(isPaddingFlagSet(block->blockHeader)){
            blockSize += 4;
            blockSize += block->padding_size;
        }
    }
    return blockSize;
}
