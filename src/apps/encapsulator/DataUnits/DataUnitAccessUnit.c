//
// Created by bscuser on 19/02/18.
//

#include "../Boxes.h"
#include "../DataStructures/BitStreams/InputBitstream.h"
#include "../DataStructures/BitStreams/OutputBitstream.h"
#include "../utils.h"
#include "DataUnits.h"

uint32_t getDataUnitAccessUnitSize(DataUnitAccessUnit* dataUnitAccessUnit, bool multipleAlignmentsFlag,
                                   uint8_t datasetType, bool pos40Bits);

DataUnitAccessUnit* initDataUnitAccessUnit(uint32_t accessUnitId, uint8_t numBlocks, uint16_t parameterSetId,
                                           ClassType AU_type, uint32_t readsCount, uint16_t mmThreshold,
                                           uint32_t mmCount, SequenceID referenceSequenceId, uint64_t referenceStartPos,
                                           uint64_t referenceEndPos, SequenceID sequenceId, uint64_t auStartPosition,
                                           uint64_t auEndPosition, uint64_t extendedAUStartPosition,
                                           uint64_t extendedAUEndPosition) {
    DataUnitAccessUnit* dataUnitAccessUnit = (DataUnitAccessUnit*)malloc(sizeof(DataUnitAccessUnit));
    if (dataUnitAccessUnit == NULL) {
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
    dataUnitAccessUnit->ref_sequenced_ID = referenceSequenceId;
    dataUnitAccessUnit->ref_start_position = referenceStartPos;
    dataUnitAccessUnit->ref_end_position = referenceEndPos;
    dataUnitAccessUnit->sequenceId = sequenceId;
    dataUnitAccessUnit->auStartPosition = auStartPosition;
    dataUnitAccessUnit->auEndPosition = auEndPosition;
    dataUnitAccessUnit->extendedAUStartPosition = extendedAUStartPosition;
    dataUnitAccessUnit->extendedAUEndPosition = extendedAUEndPosition;
    dataUnitAccessUnit->blocks = initVector();
    dataUnitAccessUnit->blocksHeaders = initVector();
    return dataUnitAccessUnit;
}

bool addBlockToDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit, Block* block,
                                  DataUnitBlockHeader* blockHeader) {
    bool blocksSuccessfulPushBack = pushBack(dataUnitAccessUnit->blocks, block);
    bool blocksHeadersSuccessfulPushBack = pushBack(dataUnitAccessUnit->blocksHeaders, blockHeader);
    if (!blocksSuccessfulPushBack || !blocksHeadersSuccessfulPushBack) {
        return false;
    }
    return true;
}

uint32_t getDataUnitAccessUnitSize(DataUnitAccessUnit* dataUnitAccessUnit, bool multipleAlignmentsFlag,
                                   uint8_t datasetType, bool pos40Bits) {
    uint32_t contentSize = 1;  // size of type
    contentSize += 4;          // size of payload's size
    contentSize += 4;          // access_unit_ID
    contentSize += 1;          // num_blocks
    contentSize += 2;          // parameter_set_ID + AU_type
    contentSize += 4;          // reads_count

    if (dataUnitAccessUnit->AU_type.classType == CLASS_TYPE_CLASS_N ||
        dataUnitAccessUnit->AU_type.classType == CLASS_TYPE_CLASS_M) {
        contentSize += 2;  // mm_threshold
        contentSize += 4;  // mm_count
    }

    if (datasetType == 2) {
        contentSize += 2;  // ref_seq
        contentSize += 4;  // ref_start
        contentSize += 4;  // ref_end
    }

    if (dataUnitAccessUnit->AU_type.classType != CLASS_TYPE_CLASS_U) {
        contentSize += 2;  // sequenceID
        uint8_t sizePos = (uint8_t)(pos40Bits ? 5 : 4);
        contentSize += sizePos;  // au_start_position
        contentSize += sizePos;  // au_end_position
        if (multipleAlignmentsFlag) {
            contentSize += sizePos;  // extended_AU_start_position
            contentSize += sizePos;  // extended_AU_end_position
        }
    } else {
        // todo unaligned case
    }

    for (size_t block_i = 0; block_i < getSize(dataUnitAccessUnit->blocks); block_i++) {
        contentSize += 1;  // descriptor_ID + reserved
        contentSize += 4;  // block_size
        Block* block = getValue(dataUnitAccessUnit->blocks, block_i);
        uint64_t blockSize = getFromFileSize(block->payload);
        contentSize += blockSize;
    }

    return contentSize;
}

int parseDataUnitAccessUnit(DataUnitAccessUnit** dataUnitAccessUnit, DataUnits* dataUnits, bool multipleAlignmentsFlag,
                            FILE* inputFile, char* filename, uint64_t size) {
    *dataUnitAccessUnit = NULL;

    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);
    uint32_t accessUnitId;
    uint8_t numBlocks;
    uint16_t parameterSetId = 0;
    uint8_t auType;
    uint32_t readsCount;
    uint16_t mmThreshold = 0;
    uint32_t mmCount = 0;
    SequenceID refSequence;
    refSequence.sequenceID = 0;
    uint64_t refStart = 0;
    uint64_t refEnd = 0;
    SequenceID sequenceId;
    sequenceId.sequenceID = 0;
    uint64_t auStartPosition = 0;
    uint64_t auEndPosition = 0;
    uint64_t extendedAuStartPosition = 0;
    uint64_t extendedAuEndPosition = 0;
    uint64_t remainingSize = size * 8;

    if (!readNBitsBigToNativeEndian32(&inputBitstream, 32, &accessUnitId) ||
        !readBytes(&inputBitstream, 1, (char*)&numBlocks) ||
        !readNBitsShift(&inputBitstream, 8, (char*)&parameterSetId) ||
        !(readNBitsShift(&inputBitstream, 4, (char*)&auType)) ||
        !(readNBitsBigToNativeEndian32(&inputBitstream, 32, &readsCount))) {
        fprintf(stderr,
                "Error reading DataUnitAccessUnit (either in access unit id, num_blocks, parameter_set_id, "
                "au_type, or read_count.\n");
        return false;
    }

    DataUnitParametersSet* parametersSet;
    if (getDataUnitsParametersById(dataUnits, parameterSetId, &parametersSet) != 0) {
        fprintf(stderr, "Error reading DataUnitAccessUnit: parameters could not be found.\n");
        return false;
    }

    remainingSize -= 32;  // accessUnitId
    remainingSize -= 8;   // numBlocks
    remainingSize -= 8;   // parameterSetId
    remainingSize -= 4;   // auType
    remainingSize -= 32;  // readsCount
    if (auType == CLASS_TYPE_CLASS_N || auType == CLASS_TYPE_CLASS_M) {
        if (!readNBitsBigToNativeEndian16(&inputBitstream, 16, &mmThreshold) ||
            !readNBitsBigToNativeEndian32(&inputBitstream, 32, &mmCount)) {
            fprintf(stderr, "Error reading DataUnitAccessUnit (either in mmThreshold or mmCount).\n");
            return false;
        }
        remainingSize -= 16;  // mmThreshold
        remainingSize -= 32;  // mmCount
    }

    if (parametersSet->encodingParameters->dataset_type == 2) {
        uint16_t refSequenceBuffer;
        if (!readNBitsBigToNativeEndian16(&inputBitstream, 16, &refSequenceBuffer)) {
            fprintf(stderr, "Error reading DataUnitAccessUnit. ref_sequence_id could not be read.\n");
        }
        refSequence.sequenceID = refSequenceBuffer;
        if (parametersSet->encodingParameters->pos_40_bits) {
            if (!readNBitsBigToNativeEndian64(&inputBitstream, 40, &refStart) ||
                !readNBitsBigToNativeEndian64(&inputBitstream, 40, &refEnd)) {
                fprintf(stderr, "Error reading DataUnitAccessUnit ref_start or ref_end could not be read.\n");
            }
        } else {
            if (!readNBitsBigToNativeEndian64(&inputBitstream, 32, &refStart) ||
                !readNBitsBigToNativeEndian64(&inputBitstream, 32, &refEnd)) {
                fprintf(stderr, "Error reading DataUnitAccessUnit ref_start or ref_end could not be read\n");
            }
        }
    }

    if (auType != CLASS_TYPE_CLASS_U) {
        if (!readNBitsBigToNativeEndian16(&inputBitstream, 16, &(sequenceId.sequenceID))) {
            fprintf(stderr, "Error reading DataUnitAccessUnit. SequenceId could not be read.\n");
            return false;
        }
        remainingSize -= 16;  // sequenceId

        bool auStartPositionSuccessfulRead;
        bool auEndPositionSuccessfulRead;
        if (parametersSet->encodingParameters->pos_40_bits) {
            auStartPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 40, &auStartPosition);
            auEndPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 40, &auEndPosition);
            remainingSize -= 5;  // auStartPosition
            remainingSize -= 5;  // auEndPosition
        } else {
            auStartPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 32, &auStartPosition);
            auEndPositionSuccessfulRead = readNBitsBigToNativeEndian64(&inputBitstream, 32, &auEndPosition);
            remainingSize -= 4;  // auStartPosition
            remainingSize -= 4;  // auEndPosition
        }
        if (!auStartPositionSuccessfulRead || !auEndPositionSuccessfulRead) {
            fprintf(stderr, "Error reading DataUnitAccessUnit. Either auStart or auEnd could not be read.\n");
            return false;
        }

        if (multipleAlignmentsFlag) {
            bool extendedAuStartPositionSuccessfulRead;
            bool extendedAuEndPositionSuccessfulRead;
            if (parametersSet->encodingParameters->pos_40_bits) {
                extendedAuStartPositionSuccessfulRead =
                    readNBitsBigToNativeEndian64(&inputBitstream, 40, &extendedAuStartPosition);
                extendedAuEndPositionSuccessfulRead =
                    readNBitsBigToNativeEndian64(&inputBitstream, 40, &extendedAuEndPosition);
                remainingSize -= 5;  // extendedAuStartPosition
                remainingSize -= 5;  // extendedAuEndPosition
            } else {
                extendedAuStartPositionSuccessfulRead =
                    readNBitsBigToNativeEndian64(&inputBitstream, 32, &extendedAuStartPosition);
                extendedAuEndPositionSuccessfulRead =
                    readNBitsBigToNativeEndian64(&inputBitstream, 32, &extendedAuEndPosition);
                remainingSize -= 4;  // extendedAuStartPosition
                remainingSize -= 4;  // extendedAuEndPosition
            }

            if (!extendedAuStartPositionSuccessfulRead || !extendedAuEndPositionSuccessfulRead) {
                fprintf(stderr,
                        "Error reading DataUnitAccessUnit either extended au start or extended au end "
                        "could not be read.\n");
                return false;
            }
        }
    }

    ClassType classType;
    classType.classType = auType;

    *dataUnitAccessUnit = initDataUnitAccessUnit(
        accessUnitId, numBlocks, parameterSetId, classType, readsCount, mmThreshold, mmCount, refSequence, refStart,
        refEnd, sequenceId, auStartPosition, auEndPosition, extendedAuStartPosition, extendedAuEndPosition);

    forwardUntilAligned(&inputBitstream);
    for (int i = 0; i < numBlocks; i++) {
        uint8_t descriptorIdAndReserved;
        uint32_t payloadSizeShifted;

        readNBitsShift(&inputBitstream, 8, (char*)&descriptorIdAndReserved);
        readNBitsBigToNativeEndian32(&inputBitstream, 32, &payloadSizeShifted);

        uint8_t descriptorId = descriptorIdAndReserved;
        uint32_t payloadSize = payloadSizeShifted;

        FromFile* fromFile = initFromFileWithFilenameAndBoundaries(filename, (uint64_t)ftell(inputFile),
                                                                   (uint64_t)(ftell(inputFile) + payloadSize));
        fseek(inputFile, payloadSize, SEEK_CUR);

        Block* block = initBlockWithHeader(descriptorId, payloadSize, fromFile);
        DataUnitBlockHeader* dataUnitBlockHeader = initDataUnitBlockHeader(descriptorId, payloadSize);

        addBlockToDataUnitAccessUnit(*dataUnitAccessUnit, block, dataUnitBlockHeader);
    }

    return true;
}

bool writeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit, bool multipleAlignmentsFlag, uint8_t datasetType,
                             bool pos40Bits, FILE* outputFile) {
    bool dataUnitAccessUnitTypeSuccessfulWrite = utils_write(2, outputFile);

    uint32_t contentSize =
        getDataUnitAccessUnitSize(dataUnitAccessUnit, multipleAlignmentsFlag, datasetType, pos40Bits);
    bool contentSizeSuccessfulWrite = writeBigEndian32ToFile(contentSize, outputFile);

    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);
    bool accessUnitIdSuccessfulWrite =
        writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->accessUnitId);
    bool numBlocksSuccessfulWrite = writeNBitsShift(&outputBitstream, 8, (char*)&(dataUnitAccessUnit->numBlocks));
    bool parameterSetIdSuccessfulWrite =
        writeNBitsShift(&outputBitstream, 8, (char*)&dataUnitAccessUnit->parameterSetId);
    bool AU_typeSuccessfulWrite = writeNBitsShift(&outputBitstream, 4, (char*)&(dataUnitAccessUnit->AU_type));
    bool readsCountSuccessfulWrite =
        writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->readsCount);
    if (!dataUnitAccessUnitTypeSuccessfulWrite || !contentSizeSuccessfulWrite || !accessUnitIdSuccessfulWrite ||
        !numBlocksSuccessfulWrite || !parameterSetIdSuccessfulWrite || !AU_typeSuccessfulWrite ||
        !readsCountSuccessfulWrite) {
        fprintf(stderr, "Error writing data unit access unit header.\n");
        return false;
    }

    if (dataUnitAccessUnit->AU_type.classType == CLASS_TYPE_CLASS_N ||
        dataUnitAccessUnit->AU_type.classType == CLASS_TYPE_CLASS_M) {
        bool mmThresholdSuccessfulWrite =
            writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 16, dataUnitAccessUnit->mmThreshold);
        bool mmCountSuccessfulWrite =
            writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, dataUnitAccessUnit->mmCount);
        if (!mmThresholdSuccessfulWrite || !mmCountSuccessfulWrite) {
            fprintf(stderr, "Error writing threshold or count.\n");
            return false;
        }
    }

    if (datasetType == 2) {
        writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 16, dataUnitAccessUnit->ref_sequenced_ID.sequenceID);
        if (pos40Bits) {
            writeNBitsShiftAndConvertToBigEndian64(&outputBitstream, 40, dataUnitAccessUnit->ref_start_position);
            writeNBitsShiftAndConvertToBigEndian64(&outputBitstream, 40, dataUnitAccessUnit->ref_end_position);
        } else {
            writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32,
                                                   (uint32_t)dataUnitAccessUnit->ref_start_position);
            writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32,
                                                   (uint32_t)dataUnitAccessUnit->ref_end_position);
        }
    }

    if (dataUnitAccessUnit->AU_type.classType != CLASS_TYPE_CLASS_U) {
        writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 16, dataUnitAccessUnit->sequenceId.sequenceID);

        bool auStartPositionSuccessfulWrite;
        bool auEndPositionSuccessfulWrite;

        if (pos40Bits) {
            auStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                &outputBitstream, 40, (uint32_t)dataUnitAccessUnit->auStartPosition);
            auEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                &outputBitstream, 40, (uint32_t)dataUnitAccessUnit->auEndPosition);
        } else {
            auStartPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                &outputBitstream, 32, (uint32_t)dataUnitAccessUnit->auStartPosition);
            auEndPositionSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                &outputBitstream, 32, (uint32_t)dataUnitAccessUnit->auEndPosition);
        }

        if (!auStartPositionSuccessfulWrite && !auEndPositionSuccessfulWrite) {
            fprintf(stderr, "Error writing au start or end.\n");
            return false;
        }
        if (multipleAlignmentsFlag) {
            bool extendedAuStartSuccessfulWrite;
            bool extendedAuEndSuccessfulWrite;
            if (pos40Bits) {
                extendedAuStartSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                    &outputBitstream, 40, dataUnitAccessUnit->extendedAUStartPosition);
                extendedAuEndSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian64(
                    &outputBitstream, 40, dataUnitAccessUnit->extendedAUEndPosition);
            } else {
                extendedAuStartSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                    &outputBitstream, 32, (uint32_t)dataUnitAccessUnit->extendedAUStartPosition);
                extendedAuEndSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(
                    &outputBitstream, 32, (uint32_t)dataUnitAccessUnit->extendedAUEndPosition);
            }
            if (!extendedAuStartSuccessfulWrite && !extendedAuEndSuccessfulWrite) {
                fprintf(stderr, "Error writing extended au start or end.\n");
                return false;
            }
        }
    }
    writeBuffer(&outputBitstream);

    for (size_t i = 0; i < getSize(dataUnitAccessUnit->blocks); i++) {
        Block* block = getValue(dataUnitAccessUnit->blocks, i);
        DataUnitBlockHeader* dataUnitBlockHeader = getValue(dataUnitAccessUnit->blocksHeaders, i);

        if (block != NULL && dataUnitBlockHeader != NULL) {
            writeDataUnitBlockHeader(outputFile, dataUnitBlockHeader);
            writeFromFile(block->payload, outputFile);
        }
    }
    return true;
}

void freeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit) {
    size_t number_blocks = getSize(dataUnitAccessUnit->blocks);
    for (size_t blocks_i = 0; blocks_i < number_blocks; blocks_i++) {
        Block* block = getValue(dataUnitAccessUnit->blocks, blocks_i);

        if (block != NULL) {
            freeBlock(block);
        }
    }
    freeVector(dataUnitAccessUnit->blocks);

    size_t number_blockHeaders = getSize(dataUnitAccessUnit->blocksHeaders);
    for (size_t blockHeaders_i = 0; blockHeaders_i < number_blockHeaders; blockHeaders_i++) {
        DataUnitBlockHeader* dataUnitBlockHeader = getValue(dataUnitAccessUnit->blocksHeaders, blockHeaders_i);

        if (dataUnitBlockHeader != NULL) {
            freeDataUnitBlockHeader(dataUnitBlockHeader);
        }
    }
    freeVector(dataUnitAccessUnit->blocksHeaders);

    free(dataUnitAccessUnit);
}

int getDataUnitAccessUnitId(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* accessUnitId) {
    if (dataUnitAccessUnit == NULL) return -1;
    *accessUnitId = dataUnitAccessUnit->accessUnitId;
    return 0;
}

int getDataUnitNumBlocks(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t* numBlocks) {
    if (dataUnitAccessUnit == NULL) return -1;
    *numBlocks = dataUnitAccessUnit->numBlocks;
    return 0;
}

int getDataUnitParameterSetId(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t* parameterSetId) {
    if (dataUnitAccessUnit == NULL) return -1;
    *parameterSetId = dataUnitAccessUnit->parameterSetId;
    return 0;
}

int getDataUnitAUType(DataUnitAccessUnit* dataUnitAccessUnit, ClassType* auType) {
    if (dataUnitAccessUnit == NULL) return -1;
    *auType = dataUnitAccessUnit->AU_type;
    return 0;
}

int getDataUnitReadsCount(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* readsCount) {
    if (dataUnitAccessUnit == NULL) return -1;
    *readsCount = dataUnitAccessUnit->readsCount;
    return 0;
}

int getDataUnitMMThreshold(DataUnitAccessUnit* dataUnitAccessUnit, uint16_t* mmThreshold) {
    if (dataUnitAccessUnit == NULL) return -1;
    *mmThreshold = dataUnitAccessUnit->mmThreshold;
    return 0;
}

int getDataUnitMMCount(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* mmCount) {
    if (dataUnitAccessUnit == NULL) return -1;
    *mmCount = dataUnitAccessUnit->mmCount;
    return 0;
}

int getDataUnitSequenceId(DataUnitAccessUnit* dataUnitAccessUnit, SequenceID* sequenceId) {
    if (dataUnitAccessUnit == NULL) return -1;
    *sequenceId = dataUnitAccessUnit->sequenceId;
    return 0;
}

int getDataUnitAuStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* auStartPosition) {
    if (dataUnitAccessUnit == NULL) return -1;
    *auStartPosition = dataUnitAccessUnit->auStartPosition;
    return 0;
}

int getDataUnitAuEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* auEndPosition) {
    if (dataUnitAccessUnit == NULL) return -1;
    *auEndPosition = dataUnitAccessUnit->auEndPosition;
    return 0;
}

int getDataUnitReferenceSequenceId(DataUnitAccessUnit* dataUnitAccessUnit, SequenceID* refSequenceID) {
    if (dataUnitAccessUnit == NULL) return -1;
    *refSequenceID = dataUnitAccessUnit->ref_sequenced_ID;
    return 0;
}

int getDataUnitReferenceStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* referenceStart) {
    if (dataUnitAccessUnit == NULL) return -1;
    *referenceStart = dataUnitAccessUnit->ref_start_position;
    return 0;
}

int getDataUnitReferenceEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* referenceEnd) {
    if (dataUnitAccessUnit == NULL) return -1;
    *referenceEnd = dataUnitAccessUnit->ref_end_position;
    return 0;
}

int getDataUnitExtendedAuStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* extendedAuStartPosition) {
    if (dataUnitAccessUnit == NULL) return -1;
    *extendedAuStartPosition = dataUnitAccessUnit->extendedAUStartPosition;
    return 0;
}

int getDataUnitExtendedAuEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* extendedAuEndPosition) {
    if (dataUnitAccessUnit == NULL) return -1;
    *extendedAuEndPosition = dataUnitAccessUnit->extendedAUEndPosition;
    return 0;
}

int getDataUnitBlock(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t block_i, Block** block) {
    if (dataUnitAccessUnit == NULL || block_i >= dataUnitAccessUnit->numBlocks) return -1;
    *block = getValue(dataUnitAccessUnit->blocks, block_i);
    return 0;
}