//
// Created by gencom on 25/07/17.
//

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../Boxes.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../utils.h"

DatasetHeader* initDatasetHeaderNoMIT(DatasetGroupId datasetGroupId, DatasetId datasetId, char* version,
                                      bool multipleAlignmentFlag, bool byteOffsetSizeFlag, bool posOffsetIsUint40,
                                      bool nonOverlappingAURange, bool blockHeaderFlag, uint16_t sequencesCount,
                                      ReferenceId referenceId, uint8_t datasetType, uint8_t numClasses,
                                      uint8_t alphabetId, uint32_t numUClusters, uint8_t uSignatureSize,
                                      uint8_t uSignatureLength, bool uSignatureConstantLength,
                                      uint32_t numberUAccessUnits, uint32_t multipleSignatureBase) {
    DatasetHeader* datasetHeader = (DatasetHeader*)malloc(sizeof(DatasetHeader));

    datasetHeader->datasetGroupId = datasetGroupId;
    datasetHeader->datasetId = datasetId;
    memcpy(datasetHeader->version, version, 4);
    datasetHeader->multipleAlignmentFlag = multipleAlignmentFlag;
    datasetHeader->byteOffsetSizeFlag = byteOffsetSizeFlag;
    datasetHeader->posOffsetIsUint40 = posOffsetIsUint40;
    datasetHeader->nonOverlappingAURange_flag = nonOverlappingAURange;
    datasetHeader->blockHeaderFlag = blockHeaderFlag;

    datasetHeader->mitFlag = false;
    datasetHeader->classContiguosModeFlag = false;
    datasetHeader->orderedBlocksFlag = false;

    datasetHeader->sequencesCount = sequencesCount;
    datasetHeader->referenceId = referenceId;
    datasetHeader->seqIds = (SequenceID*)calloc(datasetHeader->sequencesCount, sizeof(SequenceID));
    datasetHeader->seqBlocks = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->datasetType = datasetType;
    datasetHeader->numClasses = numClasses;
    datasetHeader->classId = (ClassType*)calloc(numClasses, sizeof(ClassType*));
    datasetHeader->numDescriptors = (uint8_t*)calloc(numClasses, sizeof(uint8_t));
    datasetHeader->descriptorId = (uint8_t**)calloc(numClasses, sizeof(uint8_t*));

    datasetHeader->alphabetId = alphabetId;
    datasetHeader->numUClusters = numUClusters;
    datasetHeader->uSignatureConstantLength = uSignatureConstantLength;
    datasetHeader->uSignatureSize = uSignatureSize;
    datasetHeader->uSignatureLength = uSignatureLength;
    datasetHeader->numberUAccessUnits = numberUAccessUnits;
    datasetHeader->multipleSignatureBase = multipleSignatureBase;
    datasetHeader->thresholds = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->hasSeek = false;
    datasetHeader->seekPosition = 0;
    return datasetHeader;
}

DatasetHeader* initDatasetHeader_MIT_AUC(DatasetGroupId datasetGroupId, DatasetId datasetId, char* version,
                                         bool multipleAlignmentFlag, bool byteOffsetSizeFlag, bool posOffsetIsUint40,
                                         bool nonOverlappingAURange, uint16_t sequencesCount, ReferenceId referenceId,
                                         uint8_t datasetType, uint8_t numClasses, uint8_t alphabetId,
                                         uint32_t numUClusters, bool uSignatureConstantLength, uint8_t uSignatureSize,
                                         uint8_t uSignatureLength, uint32_t numberUAccessUnits,
                                         uint32_t multipleSignatureBase, bool classContiguous) {
    DatasetHeader* datasetHeader = (DatasetHeader*)malloc(sizeof(DatasetHeader));

    datasetHeader->datasetGroupId = datasetGroupId;
    datasetHeader->datasetId = datasetId;
    memcpy(datasetHeader->version, version, 4);
    datasetHeader->multipleAlignmentFlag = multipleAlignmentFlag;
    datasetHeader->byteOffsetSizeFlag = byteOffsetSizeFlag;
    datasetHeader->posOffsetIsUint40 = posOffsetIsUint40;
    datasetHeader->nonOverlappingAURange_flag = nonOverlappingAURange;
    datasetHeader->blockHeaderFlag = true;

    datasetHeader->mitFlag = true;
    datasetHeader->classContiguosModeFlag = classContiguous;
    datasetHeader->orderedBlocksFlag = false;

    datasetHeader->sequencesCount = sequencesCount;
    datasetHeader->referenceId = referenceId;
    datasetHeader->seqIds = (SequenceID*)calloc(datasetHeader->sequencesCount, sizeof(SequenceID));
    datasetHeader->seqBlocks = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->datasetType = datasetType;
    datasetHeader->numClasses = numClasses;
    datasetHeader->classId = (ClassType*)calloc(numClasses, sizeof(ClassType*));
    datasetHeader->numDescriptors = (uint8_t*)calloc(numClasses, sizeof(uint8_t));
    datasetHeader->descriptorId = (uint8_t**)calloc(numClasses, sizeof(uint8_t*));

    datasetHeader->alphabetId = alphabetId;
    datasetHeader->numUClusters = numUClusters;
    datasetHeader->uSignatureConstantLength = uSignatureConstantLength;
    datasetHeader->uSignatureSize = uSignatureSize;
    datasetHeader->uSignatureLength = uSignatureLength;
    datasetHeader->numberUAccessUnits = numberUAccessUnits;
    datasetHeader->multipleSignatureBase = multipleSignatureBase;
    datasetHeader->thresholds = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->hasSeek = false;
    datasetHeader->seekPosition = 0;
    return datasetHeader;
}

DatasetHeader* initDatasetHeader_DSC(DatasetGroupId datasetGroupId, DatasetId datasetId, char* version,
                                     bool multipleAlignmentFlag, bool byteOffsetSizeFlag, bool posOffsetIsUint40,
                                     bool nonOverlappingAURange, uint16_t sequencesCount, ReferenceId referenceId,
                                     uint8_t datasetType, uint8_t numClasses, uint8_t alphabetId, uint32_t numUClusters,
                                     bool uSignatureConstantLength, uint8_t uSignatureSize, uint8_t uSignatureLength,
                                     uint32_t numberUAccessUnits, uint32_t multipleSignatureBase,
                                     bool orderedBlocksFlag) {
    DatasetHeader* datasetHeader = (DatasetHeader*)malloc(sizeof(DatasetHeader));

    datasetHeader->datasetGroupId = datasetGroupId;
    datasetHeader->datasetId = datasetId;
    memcpy(datasetHeader->version, version, 4);
    datasetHeader->multipleAlignmentFlag = multipleAlignmentFlag;
    datasetHeader->byteOffsetSizeFlag = byteOffsetSizeFlag;
    datasetHeader->nonOverlappingAURange_flag = nonOverlappingAURange;
    datasetHeader->posOffsetIsUint40 = posOffsetIsUint40;
    datasetHeader->mitFlag = true;
    datasetHeader->blockHeaderFlag = false;

    datasetHeader->orderedBlocksFlag = orderedBlocksFlag;

    datasetHeader->sequencesCount = sequencesCount;
    datasetHeader->referenceId = referenceId;
    datasetHeader->seqIds = (SequenceID*)calloc(datasetHeader->sequencesCount, sizeof(SequenceID));
    datasetHeader->seqBlocks = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->datasetType = datasetType;
    datasetHeader->numClasses = numClasses;
    datasetHeader->classId = (ClassType*)calloc(numClasses, sizeof(ClassType*));
    datasetHeader->numDescriptors = (uint8_t*)calloc(numClasses, sizeof(uint8_t));
    datasetHeader->descriptorId = (uint8_t**)calloc(numClasses, sizeof(uint8_t*));

    datasetHeader->alphabetId = alphabetId;
    datasetHeader->numUClusters = numUClusters;
    datasetHeader->uSignatureConstantLength = uSignatureConstantLength;
    datasetHeader->uSignatureSize = uSignatureSize;
    datasetHeader->uSignatureLength = uSignatureLength;
    datasetHeader->numberUAccessUnits = numberUAccessUnits;
    datasetHeader->multipleSignatureBase = multipleSignatureBase;
    datasetHeader->thresholds = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->hasSeek = false;
    datasetHeader->seekPosition = 0;
    return datasetHeader;
}

int setClassContiguousModeFlag(DatasetHeader* datasetHeader, bool classContiguousMode) {
    if (datasetHeader == NULL) {
        return -1;
    }
    if (!datasetHeader->blockHeaderFlag) {
        return -2;
    }
    datasetHeader->classContiguosModeFlag = classContiguousMode;
    return 0;
}

bool setSequenceId(DatasetHeader* datasetHeader, uint16_t sequenceIndex, SequenceID sequenceId) {
    if (sequenceIndex >= datasetHeader->sequencesCount) {
        return false;
    }
    datasetHeader->seqIds[sequenceIndex] = sequenceId;
    return true;
}

bool setBlocksInSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t blocks) {
    if (sequenceIndex >= datasetHeader->sequencesCount) {
        return false;
    }
    datasetHeader->seqBlocks[sequenceIndex] = blocks;
    return true;
}

bool setThresholdForSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t threshold) {
    if (sequenceIndex == 0) {
        datasetHeader->threshold_0 = threshold;
        if (sequenceIndex < datasetHeader->sequencesCount) {
            datasetHeader->thresholds[sequenceIndex] = threshold;
            return true;
        }
    }
    if (sequenceIndex >= datasetHeader->sequencesCount) {
        return false;
    }
    datasetHeader->thresholds[sequenceIndex] = threshold;
    return true;
}

bool getThresholdForSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t* threshold) {
    if (sequenceIndex == 0) {
        *threshold = datasetHeader->threshold_0;
        return true;
    }
    if (sequenceIndex >= datasetHeader->sequencesCount) {
        return false;
    }
    *threshold = datasetHeader->thresholds[sequenceIndex];
    return true;
}

bool isMultipleAlignment(DatasetHeader* datasetHeader) { return datasetHeader->multipleAlignmentFlag; }

bool isPosOffsetUint40(DatasetHeader* datasetHeader) { return datasetHeader->posOffsetIsUint40; }

bool isByteOffsetUint64(DatasetHeader* datasetHeader) { return datasetHeader->byteOffsetSizeFlag; }

bool isBlockHeaderFlagSet(DatasetHeader* datasetHeader) { return datasetHeader->blockHeaderFlag; }

DatasetType getDatasetType(DatasetHeader* datasetHeader) { return datasetHeader->datasetType; }

bool isOrderedByStartPosition(DatasetHeader* datasetHeader) { return datasetHeader->orderedBlocksFlag; }

bool isNonOverlappingAURange(DatasetHeader* datasetHeader) { return datasetHeader->nonOverlappingAURange_flag; }

uint64_t getSizeDatasetHeader(DatasetHeader* datasetHeader) {
    return BOX_HEADER_SIZE + getSizeContentDatasetHeader(datasetHeader);
}

uint64_t getSizeContentDatasetHeader(DatasetHeader* datasetHeader) {
    uint64_t contentSize = 1;  // datasetgroup id
    contentSize += 2;          // datasetId
    contentSize += 4;          // version

    uint64_t bitSize = 0;
    bitSize += 1;  // multiple_alignment_flag
    bitSize += 1;  // byte_offset_size_flag
    bitSize += 1;  // non_overlapping_AU_range
    bitSize += 1;  // pos_40_bits
    bitSize += 1;  // block_header_flag

    if (isBlockHeaderFlagSet(datasetHeader)) {
        bitSize += 1;  // MIT_flag
        bitSize += 1;  // CC_mode_flag
    } else {
        bitSize += 1;  // ordered_blocks_flag
    }
    bitSize += 16;  // sequences count
    if (getSequencesCount(datasetHeader) > 0) {
        bitSize += 8;                                                // reference_ID
        bitSize += getSequencesCount(datasetHeader) * (uint64_t)16;  // seq_ID
        bitSize += getSequencesCount(datasetHeader) * (uint64_t)32;  // seq_blocks
    }
    bitSize += 4;  // dataset_type
    if (isMITFlagSet(datasetHeader)) {
        bitSize += 4;  // num_classes
        unsigned long numClasses = getClassesCount(datasetHeader);
        for (uint8_t class_i = 0; class_i < numClasses; class_i++) {
            bitSize += 4;  // classId
            if (!datasetHeader->blockHeaderFlag) {
                bitSize += (uint64_t)5;  // num_descriptors
                uint8_t numberDescriptors = getNumberDescriptorsInClass(datasetHeader, class_i);
                bitSize += (uint64_t)7 * numberDescriptors;
            }
        }
    }

    bitSize += 8;   // alphabet_ID
    bitSize += 32;  // num_U_access_units
    if (datasetHeader->numberUAccessUnits > 0) {
        bitSize += 32;  // num_U_clusters
        bitSize += 31;  // multiple_signature_base
        if (datasetHeader->multipleSignatureBase > 0) {
            bitSize += 6;  // u_signature_size
        }
        bitSize += 1;  // u_signature_constant_length
        if (datasetHeader->uSignatureConstantLength) {
            bitSize += 8;  // u_signature_length
        }
    }

    if (datasetHeader->sequencesCount > 0) {
        bitSize += 1;   // tflag[0]
        bitSize += 31;  // thres[0]

        uint32_t currentThreshold = datasetHeader->thresholds[0];
        for (uint16_t i = 1; i < datasetHeader->sequencesCount; i++) {
            bitSize++;  // tflag
            if (datasetHeader->thresholds[i] != currentThreshold) {
                bitSize += 31;
                currentThreshold = datasetHeader->thresholds[i];
            }
        }
    }

    contentSize += bitsToBytes(bitSize);

    return contentSize;
}

bool writeDatasetHeader(DatasetHeader* datasetHeader, FILE* outputFile) {
    uint64_t datasetHeaderSize = getSizeDatasetHeader(datasetHeader);
    bool boxHeaderSuccessfulWrite = writeBoxHeader(outputFile, datasetHeaderName, datasetHeaderSize);
    if (!boxHeaderSuccessfulWrite) {
        fprintf(stderr, "Error writing type and size of Dataset Header.\n");
        return false;
    }
    return writeContentDatasetHeader(datasetHeader, outputFile);
}

bool writeContentDatasetHeader(DatasetHeader* datasetHeader, FILE* outputFile) {
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    bool datasetGroupIdSuccessfulWrite = writeToBitstream(&outputBitstream, datasetHeader->datasetGroupId);
    bool datasetIdSuccessfulWrite = writeBigEndian16ToBitstream(&outputBitstream, datasetHeader->datasetId);
    bool versionSuccessfulWrite = writeBytes(&outputBitstream, 4, datasetHeader->version);
    bool multipleAlignmentFlagSuccessfulWrite =
        writeBit(&outputBitstream, datasetHeader->multipleAlignmentFlag ? (uint8_t)1 : (uint8_t)0);
    bool byteOffsetIsUint64FlagSuccessfulWrite =
        writeBit(&outputBitstream, datasetHeader->byteOffsetSizeFlag ? (uint8_t)1 : (uint8_t)0);
    bool nonOverlappingAURangeFlagSuccessfulWrite =
        writeBit(&outputBitstream, datasetHeader->nonOverlappingAURange_flag ? (uint8_t)1 : (uint8_t)0);
    bool posOffsetIsUint40SuccessfulWrite =
        writeBit(&outputBitstream, datasetHeader->posOffsetIsUint40 ? (uint8_t)1 : (uint8_t)0);
    bool blockHeaderFlagSuccessfulWrite =
        writeBit(&outputBitstream, datasetHeader->blockHeaderFlag ? (uint8_t)1 : (uint8_t)0);

    if (!datasetGroupIdSuccessfulWrite || !datasetIdSuccessfulWrite || !versionSuccessfulWrite ||
        !multipleAlignmentFlagSuccessfulWrite || !byteOffsetIsUint64FlagSuccessfulWrite ||
        !nonOverlappingAURangeFlagSuccessfulWrite || !posOffsetIsUint40SuccessfulWrite ||
        !blockHeaderFlagSuccessfulWrite) {
        fprintf(stderr, "Error writing dataset header.\n");
        return false;
    }

    if (datasetHeader->blockHeaderFlag) {
        bool mitFlagSuccessfulWrite = writeBit(&outputBitstream, datasetHeader->mitFlag ? (uint8_t)1 : (uint8_t)0);
        bool classContigousSuccessfulWrite =
            writeBit(&outputBitstream, datasetHeader->classContiguosModeFlag ? (uint8_t)1 : (uint8_t)0);

        if (!mitFlagSuccessfulWrite || !classContigousSuccessfulWrite) {
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }
    } else {
        bool orderedBlocksFlagSuccessfulWrite =
            writeBit(&outputBitstream, datasetHeader->orderedBlocksFlag ? (uint8_t)1 : (uint8_t)0);
        if (!orderedBlocksFlagSuccessfulWrite) {
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }
    }
    writeBigEndian16ToBitstream(&outputBitstream, datasetHeader->sequencesCount);
    if (datasetHeader->sequencesCount > 0) {
        writeBytes(&outputBitstream, 1, (char*)&(datasetHeader->referenceId));
        for (typeof(datasetHeader->sequencesCount) seq_i = 0; seq_i < datasetHeader->sequencesCount; seq_i++) {
            bool seqIDSuccessfulWrite =
                writeBigEndian16ToBitstream(&outputBitstream, datasetHeader->seqIds[seq_i].sequenceID);
            if (!seqIDSuccessfulWrite) {
                fprintf(stderr, "Error writing dataset header.\n");
                return false;
            }
        }
        for (typeof(datasetHeader->sequencesCount) seq_i = 0; seq_i < datasetHeader->sequencesCount; seq_i++) {
            bool seqBlocksSuccessfulWrite =
                writeBigEndian32ToBitstream(&outputBitstream, datasetHeader->seqBlocks[seq_i]);
            if (!seqBlocksSuccessfulWrite) {
                fprintf(stderr, "Error writing dataset header.\n");
                return false;
            }
        }
    }

    if (!writeNBitsShift(&outputBitstream, 4, (char*)&(datasetHeader->datasetType))) {
        fprintf(stderr, "Error writing dataset header.\n");
        return false;
    }

    if (datasetHeader->mitFlag) {
        if (!writeNBitsShift(&outputBitstream, 4, (char*)&(datasetHeader->numClasses))) {
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }

        for (typeof(datasetHeader->numClasses) class_i = 0; class_i < datasetHeader->numClasses; class_i++) {
            bool classIdSuccessfulWrite = writeNBitsShift(&outputBitstream, 4, (char*)&datasetHeader->classId[class_i]);
            if (!classIdSuccessfulWrite) {
                fprintf(stderr, "Error writing dataset header.\n");
                return false;
            }

            if (!isBlockHeaderFlagSet(datasetHeader)) {
                bool numDescriptorsSuccessfulWrite =
                    writeNBitsShift(&outputBitstream, 5, (char*)&datasetHeader->numDescriptors[class_i]);
                for (uint8_t descriptor_i = 0; descriptor_i < datasetHeader->numDescriptors[class_i]; descriptor_i++) {
                    if (!writeNBitsShift(&outputBitstream, 7,
                                         (char*)&datasetHeader->descriptorId[class_i][descriptor_i])) {
                    };
                }
            }
        }
    }
    writeBytes(&outputBitstream, 1, (char*)&(datasetHeader->alphabetId));

    writeBigEndian32ToBitstream(&outputBitstream, datasetHeader->numberUAccessUnits);
    if (datasetHeader->numberUAccessUnits > 0) {
        writeBigEndian32ToBitstream(&outputBitstream, datasetHeader->numUClusters);
        bool multipleSignatureBaseSuccessfulWrite =
            writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 31, datasetHeader->multipleSignatureBase);
        if (!multipleSignatureBaseSuccessfulWrite) {
            fprintf(stderr, "Error writing multiple signature base.\n");
            return false;
        }

        if (datasetHeader->multipleSignatureBase > 0) {
            writeNBitsShift(&outputBitstream, 6, (char*)&datasetHeader->uSignatureSize);
        }
        writeBit(&outputBitstream, (uint8_t)(datasetHeader->uSignatureConstantLength ? 1 : 0));
        if (datasetHeader->uSignatureConstantLength) {
            writeBytes(&outputBitstream, 1, (char*)&(datasetHeader->uSignatureLength));
        }
    }

    if (datasetHeader->sequencesCount > 0) {
        bool flagThresholdWrite = writeBit(&outputBitstream, 1);
        uint32_t currentThreshold;
        getThresholdForSequence(datasetHeader, 0, &currentThreshold);
        bool thresholdWrite = writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 31, currentThreshold);
        if (!flagThresholdWrite || !thresholdWrite) {
            fprintf(stderr, "Error writing threshold.\n");
            return false;
        }
        for (uint16_t seqId = 1; seqId < datasetHeader->sequencesCount; seqId++) {
            if (datasetHeader->thresholds[seqId] != currentThreshold) {
                flagThresholdWrite = writeBit(&outputBitstream, 1);
                getThresholdForSequence(datasetHeader, seqId, &currentThreshold);
                thresholdWrite = writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 31, currentThreshold);
            } else {
                flagThresholdWrite = writeBit(&outputBitstream, 0);
            }
            if (!flagThresholdWrite || !thresholdWrite) {
                fprintf(stderr, "Error writing threshold.\n");
                return false;
            }
        }
    }

    writeBuffer(&outputBitstream);
    return true;
}

void freeDatasetHeader(DatasetHeader* datasetHeader) {
    free(datasetHeader->seqIds);
    free(datasetHeader->seqBlocks);
    free(datasetHeader->classId);
    free(datasetHeader->numDescriptors);
    if (datasetHeader->descriptorId != NULL) {
        for (typeof(datasetHeader->numClasses) class_i = 0; class_i < datasetHeader->numClasses; class_i++) {
            if (datasetHeader->descriptorId[class_i] != NULL) {
                free(datasetHeader->descriptorId[class_i]);
            }
        }
    }
    free(datasetHeader->descriptorId);
    free(datasetHeader->thresholds);
    free(datasetHeader);
}

DatasetHeader* parseDatasetHeader(FILE* inputFile) {
    DatasetGroupId datasetGroupId;
    DatasetId datasetId;
    char version[4];
    bool multipleAlignmentFlag;
    bool byteOffsetSizeFlag;
    bool posOffsetIsUint40;
    bool nonOverlappingAURange;
    bool blockHeaderFlag;
    // if blockHeaderFlag
    bool mitFlag = true;
    bool classContiguousModeFlag = false;
    bool blockStartCodePrefixFlag = false;
    // else
    bool orderedBlocksFlag = false;
    uint16_t sequencesCount;
    ReferenceId referenceId = 0;
    SequenceID* seqIds = NULL;
    uint32_t* seqBlocks = NULL;
    uint8_t datasetType;
    uint8_t numClasses = 0;
    ClassType* classId = NULL;
    uint8_t* numDescriptors = NULL;
    uint8_t** descriptorId = NULL;
    uint8_t alphabetId;
    uint32_t numUClusters = 0;
    bool uSignatureConstantLength = true;
    uint8_t uSignatureSize;
    uint8_t uSignatureLength;
    uint32_t numberUAccessUnits;
    uint32_t multipleSignatureBase = 0;
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        return NULL;
    }

    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    bool datasetGroupIdSuccessfulRead = readBytes(&inputBitstream, 1, (char*)&datasetGroupId);
    bool datasetIdSuccessfulRead = readNBitsBigToNativeEndian16(&inputBitstream, 16, &datasetId);
    bool versionSuccessfulRead = readBytes(&inputBitstream, 4, (char*)&version);
    uint8_t value;
    bool multipleAlignmentFlagSuccessfulRead = readBit(&inputBitstream, &value);
    multipleAlignmentFlag = value != 0;
    bool byteOffsetSizeFlagSuccessfulRead = readBit(&inputBitstream, &value);
    byteOffsetSizeFlag = value != 0;
    bool nonOverlappingAURangeSuccessfulRead = readBit(&inputBitstream, &value);
    nonOverlappingAURange = value != 0;
    bool posOffsetIsUint40SuccessfulRead = readBit(&inputBitstream, &value);
    posOffsetIsUint40 = value != 0;
    bool blockHeaderFlagSuccessfulRead = readBit(&inputBitstream, &value);
    blockHeaderFlag = value != 0;

    if (!datasetGroupIdSuccessfulRead || !datasetIdSuccessfulRead || !versionSuccessfulRead ||
        !multipleAlignmentFlagSuccessfulRead || !byteOffsetSizeFlagSuccessfulRead || !posOffsetIsUint40SuccessfulRead ||
        !nonOverlappingAURangeSuccessfulRead || !blockHeaderFlagSuccessfulRead) {
        fprintf(stderr, "Error reading datasetHeader.\n");
        return NULL;
    }
    if (blockHeaderFlag) {
        bool mitFlagSuccessfulRead = readBit(&inputBitstream, &value);
        mitFlag = value != 0;
        bool classContiguousModeFlagSuccessfulRead = readBit(&inputBitstream, &value);
        classContiguousModeFlag = value != 0;
    } else {
        bool orderedBlocksFlagSuccessfulRead = readBit(&inputBitstream, &value);
        orderedBlocksFlag = value != 0;
        mitFlag = true;
    }

    bool sequencesCountSuccessfulRead = readNBitsBigToNativeEndian16(&inputBitstream, 16, &sequencesCount);
    if (!sequencesCountSuccessfulRead) {
        fprintf(stderr, "Error reading sequences count.\n");
        return NULL;
    } else {
        seqIds = (SequenceID*)malloc(sequencesCount * sizeof(SequenceID));
        seqBlocks = (uint32_t*)malloc(sequencesCount * sizeof(uint32_t));
        if (seqIds == NULL || seqBlocks == NULL) {
            fprintf(stderr, "Error reading dataset header: could not allocate memory.\n");
            free(seqIds);
            free(seqBlocks);
            return NULL;
        }
    }

    if (sequencesCount > 0) {
        bool referenceIdSuccessfulRead = readBytes(&inputBitstream, 1, (char*)&referenceId);
        if (!referenceIdSuccessfulRead) {
            fprintf(stderr, "Error reading reference id.\n");
            free(seqIds);
            free(seqBlocks);
            return NULL;
        }
        bool errorReading = false;
        for (typeof(sequencesCount) seq_i = 0; seq_i < sequencesCount; seq_i++) {
            if (!readNBitsBigToNativeEndian16(&inputBitstream, 16, &(seqIds[seq_i].sequenceID))) {
                errorReading = true;
                break;
            };
        }
        if (errorReading) {
            fprintf(stderr, "Error reading sequence id.\n");
            free(seqIds);
            free(seqBlocks);
            return NULL;
        }
        for (typeof(sequencesCount) seq_i = 0; seq_i < sequencesCount; seq_i++) {
            if (!readNBitsBigToNativeEndian32(&inputBitstream, 32, &(seqBlocks[seq_i]))) {
                errorReading = true;
                break;
            };
        }
        if (errorReading) {
            fprintf(stderr, "Error reading number blocks in sequence.\n");
            free(seqIds);
            free(seqBlocks);
            return NULL;
        }
    }

    bool datasetTypeSuccessfulRead = readNBitsShift(&inputBitstream, 4, (char*)&datasetType);
    if (!datasetTypeSuccessfulRead) {
        fprintf(stderr, "Error reading dataset type.\n");
        free(seqIds);
        free(seqBlocks);
        return NULL;
    }

    if (mitFlag) {
        bool numClassesSuccessfulRead = readNBitsShift(&inputBitstream, 4, (char*)&numClasses);

        if (!numClassesSuccessfulRead) {
            fprintf(stderr, "Error reading number classes.\n");
            free(seqIds);
            free(seqBlocks);
            return NULL;
        }

        classId = (ClassType*)malloc(numClasses * sizeof(ClassType));
        descriptorId = (uint8_t**)calloc(numClasses, sizeof(uint8_t*));
        numDescriptors = calloc(numClasses, sizeof(uint8_t));
        if (!classId || !descriptorId) {
            fprintf(stderr,
                    "Error allocating memory for classId and descriptorId and numDescriptors in dataset header.\n");
            free(seqIds);
            free(seqBlocks);
            free(classId);
            free(descriptorId);
            free(numDescriptors);
            return NULL;
        }

        for (typeof(numClasses) class_i = 0; class_i < numClasses; class_i++) {
            uint8_t classTypeBuffer;
            bool classTypeSuccessfulRead = readNBitsShift(&inputBitstream, 4, (char*)&classTypeBuffer);
            if (!classTypeSuccessfulRead) {
                fprintf(stderr, "Error reading dataset header.\n");
                free(seqIds);
                free(seqBlocks);
                free(classId);
                free(descriptorId);
                return NULL;
            }
            classId[class_i].classType = classTypeBuffer;

            if (!blockHeaderFlag) {
                bool numDescriptorsSuccessfulRead = readNBitsShift(&inputBitstream, 5, (char*)numDescriptors + class_i);
                if (!numDescriptorsSuccessfulRead) {
                    fprintf(stderr, "Error reading num descriptors.\n");
                    free(seqIds);
                    free(seqBlocks);
                    free(classId);
                    free(descriptorId);
                    return NULL;
                }

                descriptorId[class_i] = (uint8_t*)malloc(sizeof(uint8_t) * numDescriptors[class_i]);
                for (uint8_t descId_i = 0; descId_i < numDescriptors[class_i]; descId_i++) {
                    bool descriptorIdSuccessfulRead =
                        readNBitsShift(&inputBitstream, 7, (char*)descriptorId[class_i] + descId_i);
                    if (!descriptorIdSuccessfulRead) {
                        fprintf(stderr, "Error reading descriptor_id.\n");
                        free(seqIds);
                        free(seqBlocks);
                        free(classId);
                        free(descriptorId);
                        return NULL;
                    }
                }
            }
        }
    }

    bool alphabetIdSuccessfulRead = readBytes(&inputBitstream, 1, (char*)&alphabetId);
    if (!alphabetIdSuccessfulRead) {
        fprintf(stderr, "Error reading alphabet id.\n");
        free(seqIds);
        free(seqBlocks);
        free(classId);
        free(descriptorId);
        return NULL;
    }

    bool numUAccessUnitsSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 32, &numberUAccessUnits);
    if (!numUAccessUnitsSuccessfulRead) {
        fprintf(stderr, "Error reading number u access units.\n");
        free(seqIds);
        free(seqBlocks);
        free(classId);
        free(descriptorId);
        return NULL;
    }

    if (numberUAccessUnits > 0) {
        bool numUClustersSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 32, &numUClusters);
        if (!numUClustersSuccessfulRead) {
            fprintf(stderr, "Error reading num clusters.\n");
            free(seqIds);
            free(seqBlocks);
            free(classId);
            free(descriptorId);
            return NULL;
        }

        bool multipleSignatureBaseSuccessfulRead =
            readNBitsBigToNativeEndian32(&inputBitstream, 31, &multipleSignatureBase);
        if (!multipleSignatureBaseSuccessfulRead) {
            fprintf(stderr, "Error reading multiple signature base.\n");
            free(seqIds);
            free(seqBlocks);
            free(classId);
            free(descriptorId);
            return NULL;
        }
        if (multipleSignatureBase > 0) {
            bool uSignatureSizeSuccessfulRead = readNBitsShift(&inputBitstream, 6, (char*)&uSignatureSize);
            if (!uSignatureSizeSuccessfulRead) {
                fprintf(stderr, "Error reading u signature size.\n");
                free(seqIds);
                free(seqBlocks);
                free(classId);
                free(descriptorId);
                return NULL;
            }
        }
        bool uSignatureConstantLengthSuccessfulRead = readBit(&inputBitstream, &value);
        if (!uSignatureConstantLengthSuccessfulRead) {
            fprintf(stderr, "Error reading u signature constant length.\n");
            free(seqIds);
            free(seqBlocks);
            free(classId);
            free(descriptorId);
            return NULL;
        }
        uSignatureConstantLength = value != 0;

        if (uSignatureConstantLength) {
            if (!readBytes(&inputBitstream, 1, (char*)&uSignatureLength)) {
                fprintf(stderr, "Error reading u signature length.\n");
                free(seqIds);
                free(seqBlocks);
                free(classId);
                free(descriptorId);
                return NULL;
            }
        } else {
            uSignatureLength = 0;
        }
    }

    DatasetHeader* datasetHeader =
        initDatasetHeaderNoMIT(datasetGroupId, datasetId, version, multipleAlignmentFlag, byteOffsetSizeFlag,
                               posOffsetIsUint40, nonOverlappingAURange, blockHeaderFlag, sequencesCount, referenceId,
                               datasetType, numClasses, alphabetId, numUClusters, uSignatureSize, uSignatureLength,
                               uSignatureConstantLength, numberUAccessUnits, multipleSignatureBase);

    if (datasetHeader == NULL) {
        fprintf(stderr, "Could not allocate memory for datasetHeader.\n");
        return datasetHeader;
    }
    datasetHeader->hasSeek = true;
    datasetHeader->seekPosition = (size_t)seekPosition;

    datasetHeader->mitFlag = mitFlag;
    datasetHeader->classContiguosModeFlag = classContiguousModeFlag;
    datasetHeader->orderedBlocksFlag = orderedBlocksFlag;
    free(datasetHeader->seqIds);
    datasetHeader->seqIds = seqIds;
    free(datasetHeader->seqBlocks);
    datasetHeader->seqBlocks = seqBlocks;
    free(datasetHeader->classId);
    datasetHeader->classId = classId;
    free(datasetHeader->numDescriptors);
    datasetHeader->numDescriptors = numDescriptors;
    free(datasetHeader->descriptorId);
    datasetHeader->descriptorId = descriptorId;

    if (sequencesCount > 0) {
        uint8_t thresholdFlag;
        uint32_t currentThreshold;
        bool thresholdFlagSuccessfulRead = readBit(&inputBitstream, &thresholdFlag);
        bool currentThresholdSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 31, &currentThreshold);

        if (!thresholdFlagSuccessfulRead || !currentThresholdSuccessfulRead) {
            fprintf(stderr, "error reading threshold information.\n");
            freeDatasetHeader(datasetHeader);
            return NULL;
        }
        if (thresholdFlag == 0) {
            fprintf(stderr, "First threshold flag has to be 1.\n");
            freeDatasetHeader(datasetHeader);
            return NULL;
        }

        setThresholdForSequence(datasetHeader, 0, currentThreshold);
        for (uint16_t sequenceId = 1; sequenceId < sequencesCount; sequenceId++) {
            thresholdFlagSuccessfulRead = readBit(&inputBitstream, &thresholdFlag);
            if (thresholdFlag != 0) {
                currentThresholdSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 31, &currentThreshold);
            }

            if (!thresholdFlagSuccessfulRead || !currentThresholdSuccessfulRead) {
                fprintf(stderr, "error reading threshold information.\n");
                freeDatasetHeader(datasetHeader);
                return NULL;
            }
            setThresholdForSequence(datasetHeader, sequenceId, currentThreshold);
        }
    }
    forwardUntilAligned(&inputBitstream);
    return datasetHeader;
}

void setDatasetType(DatasetHeader* datasetHeader, uint8_t datasetType) { datasetHeader->datasetType = datasetType; }

bool setNumberDescriptorsInClass(DatasetHeader* datasetHeader, unsigned int classId, uint8_t numberDescriptors) {
    if (datasetHeader->numClasses <= classId) return false;
    if (datasetHeader->numDescriptors == NULL) return false;
    datasetHeader->numDescriptors[classId] = numberDescriptors;

    datasetHeader->descriptorId[classId] = (uint8_t*)calloc(numberDescriptors, sizeof(uint8_t));

    return true;
}

void setClassType(DatasetHeader* datasetHeader, uint8_t classIt, ClassType classType) {
    datasetHeader->classId[classIt] = classType;
}

uint8_t getNumberDescriptorsInClass(DatasetHeader* datasetHeader, uint8_t class_index) {
    if (datasetHeader->numClasses <= class_index) return 0;
    if (datasetHeader->numDescriptors == NULL) return 0;
    return datasetHeader->numDescriptors[class_index];
}

void setDescriptorIdInClass(DatasetHeader* datasetHeader, uint8_t classIndex, uint8_t descriptorIndex,
                            uint8_t descriptorId) {
    if (datasetHeader->numClasses <= classIndex) return;
    if (datasetHeader->numDescriptors == NULL) return;
    if (datasetHeader->descriptorId == NULL) return;
    if (descriptorIndex >= datasetHeader->numDescriptors[classIndex]) return;
    datasetHeader->descriptorId[classIndex][descriptorIndex] = descriptorId;
}

uint8_t getDescriptorIdInClass(DatasetHeader* datasetHeader, uint8_t classId, uint8_t descriptorIndex) {
    if (datasetHeader->numClasses <= classId) return 0;
    if (datasetHeader->numDescriptors == NULL) return 0;
    if (datasetHeader->descriptorId == NULL) return 0;
    if (descriptorIndex >= datasetHeader->numDescriptors[classId]) return 0;
    return datasetHeader->descriptorId[classId][descriptorIndex];
}

void setConstantSignatureLength(DatasetHeader* datasetHeader, uint8_t uSignatureLength) {
    datasetHeader->uSignatureConstantLength = true;
    datasetHeader->uSignatureLength = uSignatureLength;
}

uint32_t getSignatureLength(DatasetHeader* datasetHeader) { return datasetHeader->uSignatureLength; }

uint16_t getSequencesCount(DatasetHeader* datasetHeader) { return datasetHeader->sequencesCount; }

SequenceID getSequenceId(DatasetHeader* datasetHeader, uint16_t sequence_index) {
    return datasetHeader->seqIds[sequence_index];
}

bool getSequenceIndex(DatasetHeader* datasetHeader, SequenceID sequence_id, uint16_t* sequence_index) {
    for (uint8_t seq_it = 0; seq_it < datasetHeader->sequencesCount; seq_it++) {
        if (datasetHeader->seqIds[seq_it].sequenceID == sequence_id.sequenceID) {
            *sequence_index = seq_it;
            return true;
        }
    }
    return false;
}

uint8_t getClassesCount(DatasetHeader* datasetHeader) { return datasetHeader->numClasses; }

bool hasClassType(DatasetHeader* datasetHeader, ClassType classType) {
    for (uint16_t class_i = 0; class_i < datasetHeader->numClasses; class_i++) {
        if (datasetHeader->classId[class_i].classType == classType.classType) {
            return true;
        }
    }
    return false;
}

ClassType getClassType(DatasetHeader* datasetHeader, uint16_t class_i) { return datasetHeader->classId[class_i]; }

bool getClassIndexForType(DatasetHeader* datasetHeader, ClassType classType, uint8_t* class_i) {
    for (uint8_t class_it = 0; class_it < datasetHeader->numClasses; class_it++) {
        if (datasetHeader->classId[class_it].classType == classType.classType) {
            *class_i = class_it;
            return true;
        }
    }
    return false;
}

bool getDescriptorIndexForType(DatasetHeader* datasetHeader, ClassType classType, uint8_t descriptorId,
                               uint8_t* descriptorIndex) {
    uint8_t classIndex;
    getClassIndexForType(datasetHeader, classType, &classIndex);
    uint8_t numberDescriptors = getNumberDescriptorsInClass(datasetHeader, classIndex);
    for (uint8_t descriptor_index = 0; descriptor_index < numberDescriptors; descriptor_index++) {
        if (datasetHeader->descriptorId[classIndex][descriptor_index] == descriptorId) {
            *descriptorIndex = descriptor_index;
            return true;
        }
    }
    return false;
}

uint32_t getBlocksInSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex) {
    if (sequenceIndex >= datasetHeader->sequencesCount) {
        return 0;
    }
    return datasetHeader->seqBlocks[sequenceIndex];
}

uint32_t getNumberUAccessUnits(DatasetHeader* datasetHeader) { return datasetHeader->numberUAccessUnits; }

uint32_t getUSignatureSize(DatasetHeader* datasetHeader) { return datasetHeader->uSignatureSize; }

uint32_t getMultipleSignatureBase(DatasetHeader* datasetHeader) { return datasetHeader->multipleSignatureBase; }

void setMultipleSignatureBase(DatasetHeader* datasetHeader, uint32_t newMultipleSignatureBase) {
    datasetHeader->multipleSignatureBase = newMultipleSignatureBase;
}

size_t getDatasetHeaderSeekPosition(DatasetHeader* datasetHeader) { return datasetHeader->seekPosition; }

bool isMITFlagSet(DatasetHeader* datasetHeader) {
    if (isBlockHeaderFlagSet(datasetHeader)) {
        return datasetHeader->mitFlag;
    } else {
        return true;
    }
}

ReferenceId getReferenceId(DatasetHeader* datasetHeader) { return datasetHeader->referenceId; }

void setMITFlag(DatasetHeader* datasetHeader, bool value) { datasetHeader->mitFlag = value; }
