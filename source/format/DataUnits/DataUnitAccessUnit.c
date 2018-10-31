//
// Created by bscuser on 19/02/18.
//

#include <utils.h>
#include <DataStructures/BitStreams/OutputBitstream.h>
#include <Boxes.h>
#include <DataStructures/BitStreams/InputBitstream.h>
#include "DataUnits.h"

DataUnitAccessUnit* initDataUnitAccessUnit(
        uint32_t accessUnitId,
        uint8_t numBlocks,
        uint16_t parameterSetId,
        uint8_t AU_type,
        uint32_t readsCount,
        uint16_t mmThreshold,
        uint32_t mmCount,
        uint16_t sequenceId,
        uint64_t auStartPosition,
        uint64_t auEndPosition,
        uint64_t extendedAUStartPosition,
        uint64_t extendedAUEndPosition
){
    DataUnitAccessUnit* dataUnitAccessUnit = (DataUnitAccessUnit*)malloc(sizeof(DataUnitAccessUnit));
    if(dataUnitAccessUnit == NULL){
        fprintf(stderr, "DataUnitAccessUnit could not be allocated.\n");
        return NULL;
    }
    dataUnitAccessUnit->accessUnitId = accessUnitId;
    dataUnitAccessUnit->numBlocks = numBlocks;
    dataUnitAccessUnit->parameterSetId = parameterSetId;
    dataUnitAccessUnit->AU_type = AU_type;
    dataUnitAccessUnit->readsCount = readsCount;
    dataUnitAccessUnit->mmThreshold = mmThreshold;
    dataUnitAccessUnit->mmCount = mmCount;
    dataUnitAccessUnit->sequenceId = sequenceId;
    dataUnitAccessUnit->auStartPosition = auStartPosition;
    dataUnitAccessUnit->auEndPosition = auEndPosition;
    dataUnitAccessUnit->extendedAUStartPosition = extendedAUStartPosition;
    dataUnitAccessUnit->extendedAUEndPosition = extendedAUEndPosition;
    dataUnitAccessUnit->blocks = initVector();
    dataUnitAccessUnit->blocksHeaders = initVector();
    return dataUnitAccessUnit;
}

bool addBlockToDataUnitAccessUnit(DataUnitAccessUnit *dataUnitAccessUnit, Block *block,
                                  DataUnitBlockHeader *blockHeader){
    bool blocksSuccessfulPushBack = pushBack(dataUnitAccessUnit->blocks, block);
    bool blocksHeadersSuccessfulPushBack = pushBack(dataUnitAccessUnit->blocksHeaders, blockHeader);
    if (!blocksSuccessfulPushBack || !blocksHeadersSuccessfulPushBack){
        return false;
    }
    return true;
}

uint32_t getDataUnitAccessUnitSize(DataUnitAccessUnit* dataUnitAccessUnit, bool multipleAlignmentsFlag){
    uint32_t contentSize = 1; //size of type
    contentSize += 4; //size of payload's size
    contentSize += 13; //fixed Access unit header

    //todo unaccounted for
    contentSize += 1;

    contentSize += dataUnitAccessUnit->AU_type == N_TYPE_AU || dataUnitAccessUnit->AU_type == M_TYPE_AU ? 6 : 0;
    contentSize += dataUnitAccessUnit->AU_type != U_TYPE_AU ? 8:0;
    contentSize += dataUnitAccessUnit->AU_type != U_TYPE_AU && multipleAlignmentsFlag ? 8:0;

    for(size_t block_i=0; block_i<getSize(dataUnitAccessUnit->blocks); block_i++){
        contentSize += 6; //
        Block* block = getValue(dataUnitAccessUnit->blocks, block_i);
        long blockSize = getFromFileSize(block->payload);
        contentSize += blockSize;
    }

    return contentSize;
}

bool writeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit, bool multipleAlignmentsFlag, FILE* outputFile){
    bool dataUnitAccessUnitTypeSuccessfulWrite = write(2, outputFile);

    uint32_t contentSize = getDataUnitAccessUnitSize(dataUnitAccessUnit, multipleAlignmentsFlag);
    bool contentSizeSuccessfulWrite = writeBigEndian32ToFile(contentSize<<3, outputFile);

    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);
    bool accessUnitIdSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->accessUnitId);
    bool numBlocksSuccessfulWrite = writeNBitsShift(&outputBitstream, 8, (char*)&(dataUnitAccessUnit->numBlocks));
    bool parameterSetIdSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 12, dataUnitAccessUnit->parameterSetId);
    bool AU_typeSuccessfulWrite = writeNBitsShift(&outputBitstream, 4, (char*)&(dataUnitAccessUnit->AU_type));
    bool readsCountSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->readsCount);
    if(
        !dataUnitAccessUnitTypeSuccessfulWrite ||
        !contentSizeSuccessfulWrite ||
        !accessUnitIdSuccessfulWrite ||
        !numBlocksSuccessfulWrite ||
        !parameterSetIdSuccessfulWrite ||
        !AU_typeSuccessfulWrite ||
        !readsCountSuccessfulWrite
    ){
        fprintf(stderr, "Error writing data unit access unit header.\n");
        return false;
    }

    if(dataUnitAccessUnit->AU_type == N_TYPE_AU ||  dataUnitAccessUnit->AU_type == M_TYPE_AU){
        bool mmThresholdSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 16, dataUnitAccessUnit->mmThreshold);
        bool mmCountSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->mmCount);
        if (!mmThresholdSuccessfulWrite || !mmCountSuccessfulWrite){
            fprintf(stderr, "Error writing threshold or count.\n");
            return false;
        }
    }

    uint32_t copySequenceId = dataUnitAccessUnit->sequenceId;
    writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 24, copySequenceId, false);

    //todo unaccounted for
    //uint8_t bufferZero = 0;
    //writeNBitsShift(&outputBitstream, 8, (const char *) &bufferZero);

    if(dataUnitAccessUnit->AU_type != U_TYPE_AU){
        bool auStartPositionSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->auStartPosition);
        bool auEndPositionSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->auEndPosition);
        if (!auStartPositionSuccessfulWrite && !auEndPositionSuccessfulWrite){
            fprintf(stderr, "Error writing au start or end.\n");
            return false;
        }
        if (multipleAlignmentsFlag){
            bool extendedAuStartSuccessfulWrite =
                    writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->extendedAUStartPosition);
            bool extendedAuEndSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->extendedAUEndPosition);
            if(!extendedAuStartSuccessfulWrite && !extendedAuEndSuccessfulWrite){
                fprintf(stderr, "Error writing extended au start or end.\n");
                return false;
            }
        }
    }
    writeBuffer(&outputBitstream);

    for(int i=0; i<getSize(dataUnitAccessUnit->blocks); i++){
        Block* block = getValue(dataUnitAccessUnit->blocks, i);
        DataUnitBlockHeader* dataUnitBlockHeader = getValue(dataUnitAccessUnit->blocksHeaders, i);

        if(block != NULL && dataUnitBlockHeader != NULL){
            writeDataUnitBlockHeader(outputFile, dataUnitBlockHeader);
            writeFromFile(block->payload, outputFile);
        }
    }
    if(dataUnitAccessUnit->auStartPosition == 10005){
        for(int i=0; i<getSize(dataUnitAccessUnit->blocks); i++){
            Block* block = getValue(dataUnitAccessUnit->blocks, i);
            DataUnitBlockHeader* dataUnitBlockHeader = getValue(dataUnitAccessUnit->blocksHeaders, i);

            char* filename = calloc(500, sizeof(char));
            sprintf(filename, "/home/bscuser/data/block_%hu", dataUnitBlockHeader->descriptorId);
            FILE *outputBlock = fopen(filename, "wb");
            writeFromFile(block->payload, outputBlock);
            fclose(outputBlock);
            free(filename);
        }
    }
    return true;
}

void freeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit){
    size_t number_blockHeaders = getSize(dataUnitAccessUnit->blocksHeaders);

    freeVector(dataUnitAccessUnit->blocks);

    for(size_t blockHeaders_i = 0; blockHeaders_i < number_blockHeaders; blockHeaders_i++){
        DataUnitBlockHeader* dataUnitBlockHeader = getValue(dataUnitAccessUnit->blocksHeaders, blockHeaders_i);

        if(dataUnitBlockHeader != NULL){
            freeDataUnitBlockHeader(dataUnitBlockHeader);
        }
    }
    freeVector(dataUnitAccessUnit->blocksHeaders);


    free(dataUnitAccessUnit);
}
/*
DataUnitAccessUnit* parseDataUnitAccessUnit(FILE* inputFile, uint64_t size){
    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);
    uint32_t accessUnitId;
    uint8_t numBlocks;
    uint16_t parameterSetId;
    uint8_t auType;
    uint32_t  readsCount;
    uint16_t mmThreshold;
    uint32_t mmCount;
    uint16_t sequenceId;
    uint64_t auStartPosition;
    uint64_t auEndPosition;
    uint64_t extendedAuStartPosition;
    uint64_t extendedAuEndPosition;

    bool accessUnitIdSuccessfulRead =
            readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&accessUnitId);
    bool numBlocksSuccessfulRead = readNBitsShift(&inputBitstream, 8, (char*)&numBlocks);
    bool parameterSetIdSuccessfulRead =
            readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 12, (char*)&parameterSetId);
    bool AU_typeSuccessfulRead = readNBitsShift(&inputBitstream, 4, (char*)&(auType));
    bool readsCountSuccessfulRead =
            readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&readsCount);

    if(
        !accessUnitIdSuccessfulRead ||
        !numBlocksSuccessfulRead ||
        !parameterSetIdSuccessfulRead ||
        !AU_typeSuccessfulRead ||
        !readsCountSuccessfulRead
    ){
        fprintf(stderr, "Error reading data unit access unit header.\n");
        return false;
    }

    if(auType == N_TYPE_AU ||  auType == M_TYPE_AU){
        bool mmThresholdSuccessfulRead =
                readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16, (char*)&mmThreshold);
        bool mmCountSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&mmCount);

        if (!mmThresholdSuccessfulRead || !mmCountSuccessfulRead){
            fprintf(stderr, "Error reading threshold or count.\n");
            return false;
        }
    }

    if(auType != U_TYPE_AU){
        bool sequenceIdSuccessfulRead =
                readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16, (char*)&sequenceId);
        //todo correct posSize check
        bool posSizeIs40bits = false;

        bool auStartPositionSuccessfulRead;
        bool auEndPositionSuccessfulRead;
        bool auExtendedStartPositionSuccessfulRead;
        bool auExtendedEndPositionSuccessfulRead;

        if(!posSizeIs40bits){
            auStartPositionSuccessfulRead =
                readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&auStartPosition);
            auEndPositionSuccessfulRead =
                    readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char*)&auEndPosition);
        }else{
            auStartPositionSuccessfulRead =
                    readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 40, (char*)&auStartPosition);
            auEndPositionSuccessfulRead =
                    readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 40, (char*)&auEndPosition);
        }

        //todo correct multipleAlignment
        bool multipleAlignment = false;
        if (multipleAlignment){
            if(!posSizeIs40bits){
                auExtendedStartPositionSuccessfulRead =
                    readNBitsShiftAndConvertBigToNativeEndian32(
                        &inputBitstream,
                        32,
                        (char*)&extendedAuStartPosition
                    );
                auExtendedEndPositionSuccessfulRead =
                    readNBitsShiftAndConvertBigToNativeEndian32(
                        &inputBitstream,
                        32,
                        (char*)&extendedAuEndPosition
                    );
            }else{
                auExtendedStartPositionSuccessfulRead =
                    readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 40, (char*)&extendedAuStartPosition);
                auExtendedEndPositionSuccessfulRead =
                    readNBitsShiftAndConvertBigToNativeEndian64(&inputBitstream, 40, (char*)&extendedAuEndPosition);
            }
        }else{
            //todo getMultipleSignatureBase
            uint32_t multiple
        }




        bool auStartPositionSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->auStartPosition);
        bool auEndPositionSuccessfulWrite =
                writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->auEndPosition);
        if (!auStartPositionSuccessfulWrite && !auEndPositionSuccessfulWrite){
            fprintf(stderr, "Error writing au start or end.\n");
            return false;
        }
        if (multipleAlignmentsFlag){
            bool extendedAuStartSuccessfulWrite =
                    writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->extendedAUStartPosition);
            bool extendedAuEndSuccessfulWrite =
                    writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->extendedAUEndPosition);
            if(!extendedAuStartSuccessfulWrite && !extendedAuEndSuccessfulWrite){
                fprintf(stderr, "Error writing extended au start or end.\n");
                return false;
            }
        }
    }
    writeBuffer(&outputBitstream);

    for(int i=0; i<getSize(dataUnitAccessUnit->blocks); i++){
        Block* block = getValue(dataUnitAccessUnit->blocks, i);
        DataUnitBlockHeader* dataUnitBlockHeader = getValue(dataUnitAccessUnit->blocksHeaders, i);

        if(block != NULL && dataUnitBlockHeader != NULL){
            writeDataUnitBlockHeader(outputFile, dataUnitBlockHeader);
            writeFromFile(block->payload, outputFile);
        }
    }
    if(dataUnitAccessUnit->auStartPosition == 10005){
        for(int i=0; i<getSize(dataUnitAccessUnit->blocks); i++){
            Block* block = getValue(dataUnitAccessUnit->blocks, i);
            DataUnitBlockHeader* dataUnitBlockHeader = getValue(dataUnitAccessUnit->blocksHeaders, i);

            char* filename = calloc(500, sizeof(char));
            sprintf(filename, "/home/bscuser/data/block_%hu", dataUnitBlockHeader->descriptorId);
            FILE *outputBlock = fopen(filename, "wb");
            writeFromFile(block->payload, outputBlock);
            fclose(outputBlock);
            free(filename);
        }
    }
    return true;
}*/