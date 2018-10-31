//
// Created by gencom on 25/07/17.
//

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../Boxes.h"
#include "../../utils.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"

DatasetHeader *initDatasetHeader(DatasetGroupId datasetGroupId, DatasetId datasetId,
                                 char* version, bool unmapped_indexing_flag, bool byteOffsetSizeFlag,
                                 bool posOffsetIsUint40, bool nonOverlappingAURange, bool blockHeaderFlag,
                                 uint16_t sequencesCount, ReferenceId referenceId, uint8_t datasetType,
                                 uint8_t numClasses, uint8_t alphabetId, uint32_t numUClusters,
                                 bool uSignatureConstantLength, uint8_t uSignatureSize, uint8_t uSignatureLength,
                                 uint32_t numberUAccessUnits, bool multipleAlignmentFlag,
                                 uint32_t multipleSignatureBase
){
    DatasetHeader* datasetHeader = (DatasetHeader*)malloc(sizeof(DatasetHeader));

    datasetHeader->datasetGroupId = datasetGroupId;
    datasetHeader->datasetId = datasetId;
    memcpy(datasetHeader->version, version, 4);
    datasetHeader->unmapped_indexing_flag = unmapped_indexing_flag;
    datasetHeader->byteOffsetSizeFlag = byteOffsetSizeFlag;
    datasetHeader->posOffsetIsUint40 = posOffsetIsUint40;
    datasetHeader->nonOverlappingAURange = nonOverlappingAURange;
    datasetHeader->blockHeaderFlag = blockHeaderFlag;

    datasetHeader->mitFlag = false;
    datasetHeader->classContiguosModeFlag = false;
    datasetHeader->blockStartCodePrefixFlag = false;
    datasetHeader->orderedBlocksFlag = false;

    datasetHeader->sequencesCount = sequencesCount;
    datasetHeader->referenceId = referenceId;
    datasetHeader->seqIds = (uint16_t*)calloc(datasetHeader->sequencesCount, sizeof(uint16_t));
    datasetHeader->seqBlocks = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->datasetType=datasetType;
    datasetHeader->numClasses=numClasses;
    datasetHeader->classId = (uint8_t*)calloc(numClasses, sizeof(uint8_t*));
    datasetHeader->numDescriptors = (uint8_t*)calloc(numClasses, sizeof(uint8_t));
    datasetHeader->descriptorId = (uint8_t**)calloc(numClasses, sizeof(uint8_t*));

    datasetHeader->alphabetId = alphabetId;
    datasetHeader->numUClusters = numUClusters;
    datasetHeader->uSignatureConstantLength = uSignatureConstantLength;
    datasetHeader->uSignatureSize = uSignatureSize;
    datasetHeader->uSignatureLength = uSignatureLength;
    datasetHeader->numberUAccessUnits = numberUAccessUnits;
    datasetHeader->multipleAlignmentFlag = multipleAlignmentFlag;
    datasetHeader->multipleSignatureBase = multipleSignatureBase;
    datasetHeader->thresholds = (uint32_t*)calloc(datasetHeader->sequencesCount, sizeof(uint32_t));
    datasetHeader->seekPosition = -1;
    return datasetHeader;
}

bool setSequenceId(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint16_t sequenceId){
    if(sequenceIndex>=datasetHeader->sequencesCount){
        return false;
    }
    datasetHeader->seqIds[sequenceIndex]=sequenceId;
    return true;
}

bool setBlocksInSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t blocks){
    if(sequenceIndex>=datasetHeader->sequencesCount){
        return false;
    }
    datasetHeader->seqBlocks[sequenceIndex]=blocks;
    return true;

}

bool setThresholdForSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t threshold){
    if(sequenceIndex>=datasetHeader->sequencesCount){
        return false;
    }
    datasetHeader->thresholds[sequenceIndex]=threshold;
    return true;

}

bool isMultipleAlignment(DatasetHeader* datasetHeader){
    return datasetHeader->multipleAlignmentFlag;
}

bool isPosOffsetUint40(DatasetHeader *datasetHeader){
    return datasetHeader->posOffsetIsUint40;
}

bool isByteOffsetUint64(DatasetHeader *datasetHeader){
    return datasetHeader->byteOffsetSizeFlag;
}

bool isBlockHeaderFlagSet(DatasetHeader* datasetHeader){
    return datasetHeader->blockHeaderFlag;
}

bool isUnmappedIndexingFlagSet(DatasetHeader* datasetHeader){
    return datasetHeader->unmapped_indexing_flag;
}

DatasetType getDatasetType(DatasetHeader* datasetHeader){
    return datasetHeader->datasetType;
}

bool isOrderedByStartPosition(DatasetHeader* datasetHeader){
    return datasetHeader->orderedBlocksFlag;
}

bool isNonOverlappingAURange(DatasetHeader* datasetHeader) {
    return datasetHeader->nonOverlappingAURange;
}

uint64_t getSizeDatasetHeader(DatasetHeader* datasetHeader){
    return BOX_HEADER_SIZE+getSizeContentDatasetHeader(datasetHeader);
}

uint64_t getSizeContentDatasetHeader(DatasetHeader* datasetHeader) {
    uint64_t contentSize = 1; //datasetgroup id
    contentSize += 2; //datasetId
    contentSize += 4; //version

    uint64_t bitSize = 0;
    bitSize += 1; //unmapped_indexing_flag
    bitSize += 1; //byte_offset_size_flag
    bitSize += 1; //non_overlapping_AU_range
    bitSize += 1; //pos_40_bits
    bitSize += 1; //block_header_flag

    if(isBlockHeaderFlagSet(datasetHeader)){
        bitSize += 1;//MIT_flag
        bitSize += 1;//CC_mode_flag
    }else{
        bitSize += 1;//ordered_blocks_flag
    }
    bitSize += 16;//sequences count
    bitSize += 8; //reference_ID
    bitSize += getSequencesCount(datasetHeader)*16; //seq_ID
    bitSize += getSequencesCount(datasetHeader)*32; //seq_blocks
    bitSize += 4; //dataset_type
    bitSize += 4; //num_classes

    unsigned long numClasses = getClassesCount(datasetHeader);
    for(uint8_t class_i = 0; class_i<numClasses; class_i++){
        bitSize += 4; //classId
        bitSize += 5; //num_descriptors

        uint8_t numberDescriptors = getNumberDescriptorsInClass(datasetHeader, class_i);
        bitSize += 7 * numberDescriptors;
    }

    bitSize += 8; //alphabet_ID
    bitSize += 32; //num_U_clusters
    bitSize += 1; //u_signature_constant_length
    bitSize += 8; //u_signature_size

    if(datasetHeader->uSignatureConstantLength){
        bitSize += 8; //u_signature_length
    }

    bitSize += 32; //num_u_access_units
    bitSize += 1; //multiple_alignment_flag
    bitSize += 31; //multiple_signature_base

    bitSize += 1; //tflag[0]
    bitSize += 31; //thres[0]

    uint32_t currentThreshold = datasetHeader->thresholds[0];
    for(uint16_t i=1; i<datasetHeader->sequencesCount; i++){
        bitSize++; //tflag
        if(datasetHeader->thresholds[i]!=currentThreshold){
            bitSize+=31;
            currentThreshold = datasetHeader->thresholds[i];
        }
    }

    contentSize += bitsToBytes(bitSize);

    return contentSize;
}

bool writeDatasetHeader(DatasetHeader* datasetHeader, FILE* outputFile) {
    uint64_t datasetHeaderSize = getSizeDatasetHeader(datasetHeader);
    bool boxHeaderSuccessfulWrite = writeBoxHeader(outputFile, datasetHeaderName, datasetHeaderSize);
    if(!boxHeaderSuccessfulWrite){
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
    bool unmappedIndexingFlagSuccessfulWrite = writeBit(&outputBitstream,
                                                     datasetHeader->unmapped_indexing_flag?(uint8_t)1:(uint8_t)0);
    bool byteOffsetIsUint64FlagSuccessfulWrite = writeBit(&outputBitstream,
                                                         datasetHeader->posOffsetIsUint40?(uint8_t)1:(uint8_t)0);
    bool nonOverlappingAURangeFlagSuccessfulWrite = writeBit(&outputBitstream,
                                                        datasetHeader->nonOverlappingAURange?(uint8_t)1:(uint8_t)0);
    bool posOffsetIsUint40SuccessfulWrite = writeBit(&outputBitstream,
            datasetHeader->posOffsetIsUint40?(uint8_t)1:(uint8_t)0);
    bool blockHeaderFlagSuccessfulWrite = writeBit(&outputBitstream,
                                                         datasetHeader->blockHeaderFlag?(uint8_t)1:(uint8_t)0);

    if (
        !datasetGroupIdSuccessfulWrite ||
        !datasetIdSuccessfulWrite ||
        !versionSuccessfulWrite ||
        !unmappedIndexingFlagSuccessfulWrite ||
        !byteOffsetIsUint64FlagSuccessfulWrite ||
        !nonOverlappingAURangeFlagSuccessfulWrite ||
        !posOffsetIsUint40SuccessfulWrite ||
        !blockHeaderFlagSuccessfulWrite
    ){
        fprintf(stderr, "Error writing dataset header.\n");
        return false;
    }

    if(datasetHeader->blockHeaderFlag){
        bool mitFlagSuccessfulWrite = writeBit(&outputBitstream, datasetHeader->mitFlag?(uint8_t)1:(uint8_t)0);
        bool classContigousSuccessfulWrite = writeBit(&outputBitstream, datasetHeader->classContiguosModeFlag?(uint8_t)1:(uint8_t)0);

        if (
            !mitFlagSuccessfulWrite ||
            !classContigousSuccessfulWrite
        ){
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }
    }else{
        bool orderedBlocksFlagSuccessfulWrite = writeBit(&outputBitstream, datasetHeader->orderedBlocksFlag?(uint8_t)1:(uint8_t)0);
        if(!orderedBlocksFlagSuccessfulWrite){
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }
    }
    writeBigEndian16ToBitstream(&outputBitstream, datasetHeader->sequencesCount);
    writeBytes(&outputBitstream,1,(char*)&(datasetHeader->referenceId));
    for(typeof(datasetHeader->sequencesCount) seq_i=0; seq_i<datasetHeader->sequencesCount; seq_i++){
        bool seqIDSuccessfulWrite = writeBigEndian16ToBitstream(&outputBitstream, datasetHeader->seqIds[seq_i]);
        if(!seqIDSuccessfulWrite){
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }
    }
    for(typeof(datasetHeader->sequencesCount) seq_i=0; seq_i<datasetHeader->sequencesCount; seq_i++){
        bool seqBlocksSuccessfulWrite = writeBigEndian32ToBitstream(&outputBitstream, datasetHeader->seqBlocks[seq_i]);
        if(!seqBlocksSuccessfulWrite){
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }
    }

    if(!writeNBitsShift(&outputBitstream,4,(char*)&(datasetHeader->datasetType))){
        fprintf(stderr, "Error writing dataset header.\n");
        return false;
    }

    if(!writeNBitsShift(&outputBitstream,4,(char*)&(datasetHeader->numClasses))){
        fprintf(stderr, "Error writing dataset header.\n");
        return false;
    }
    for(typeof(datasetHeader->numClasses) class_i=0; class_i<datasetHeader->numClasses; class_i++){
        bool classIdSuccessfulWrite = writeNBitsShift(&outputBitstream,4,(char*)&datasetHeader->classId[class_i]);
        if(!classIdSuccessfulWrite){
            fprintf(stderr, "Error writing dataset header.\n");
            return false;
        }

        bool numDescriptorsSuccessfulWrite = writeNBitsShift(&outputBitstream,5,(char*)&datasetHeader->numDescriptors[class_i]);
        for(uint8_t descriptor_i=0; descriptor_i<datasetHeader->numDescriptors[class_i]; descriptor_i++){
            if(!writeNBitsShift(&outputBitstream,7,(char*)&datasetHeader->descriptorId[class_i][descriptor_i]));
        }
    }
    writeBytes(&outputBitstream,1,(char*)&(datasetHeader->alphabetId));
    writeBigEndian32ToBitstream(&outputBitstream, datasetHeader->numUClusters);
    writeBit(&outputBitstream, (uint8_t) (datasetHeader->uSignatureConstantLength?1:0));
    writeBytes(&outputBitstream,1, (char*)&(datasetHeader->uSignatureSize));
    if(datasetHeader->uSignatureConstantLength){
        writeBytes(&outputBitstream,1, (char*)&(datasetHeader->uSignatureLength));
    }
    writeBigEndian32ToBitstream(&outputBitstream, datasetHeader->numberUAccessUnits);
    writeBit(&outputBitstream,(uint8_t)(datasetHeader->multipleAlignmentFlag?1:0));

    bool multipleSignatureBaseSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 31,
                                                                                           datasetHeader->multipleSignatureBase,
                                                                                           NULL);
    if (!multipleSignatureBaseSuccessfulWrite){
        fprintf(stderr,"Error writing multiple signature base.\n");
        return false;
    }



    bool flagThresholdWrite = writeBit(&outputBitstream,1);
    uint32_t currentThreshold = datasetHeader->thresholds[0];
    bool thresholdWrite = writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 31, currentThreshold,true);
    if (!flagThresholdWrite || !thresholdWrite){
        fprintf(stderr, "Error writing threshold.\n");
        return false;
    }
    for(uint16_t seqId=1; seqId<datasetHeader->sequencesCount; seqId++){
        if(datasetHeader->thresholds[seqId]!=currentThreshold){
            flagThresholdWrite = writeBit(&outputBitstream,1);
            currentThreshold = datasetHeader->thresholds[seqId];
            thresholdWrite = writeNBitsShiftAndConvertToBigEndian32_new(&outputBitstream, 31, currentThreshold, NULL);
        }else{
            flagThresholdWrite = writeBit(&outputBitstream,0);
        }
        if (!flagThresholdWrite || !thresholdWrite){
            fprintf(stderr, "Error writing threshold.\n");
            return false;
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
    if(datasetHeader->descriptorId != NULL) {
        for (typeof(datasetHeader->numClasses) class_i = 0; class_i < datasetHeader->numClasses; class_i++) {
            if(datasetHeader->descriptorId[class_i]!=NULL){
                free(datasetHeader->descriptorId[class_i]);
            }
        }
    }
    free(datasetHeader->descriptorId);
    free(datasetHeader->thresholds);
    free(datasetHeader);
}

DatasetHeader *parseDatasetHeader(FILE *inputFile) {
    DatasetGroupId datasetGroupId;
    DatasetId datasetId;
    char version[4];
    bool unmapped_indexing_flag;
    bool byteOffsetSizeFlag;
    bool posOffsetIsUint40;
    bool nonOverlappingAURange;
    bool blockHeaderFlag;
    //if blockHeaderFlag
    bool mitFlag = false;
    bool classContiguousModeFlag = false;
    bool blockStartCodePrefixFlag = false;
    //else
    bool orderedBlocksFlag = false;
    uint16_t sequencesCount;
    ReferenceId referenceId;
    uint16_t* seqIds = NULL;
    uint32_t* seqBlocks = NULL;
    uint8_t datasetType;
    uint8_t numClasses;
    uint8_t* classId = NULL;
    uint8_t* numDescriptors = NULL;
    uint8_t** descriptorId = NULL;
    uint8_t alphabetId;
    uint32_t numUClusters;
    bool uSignatureConstantLength;
    uint8_t uSignatureSize;
    uint8_t uSignatureLength;
    uint32_t numberUAccessUnits;
    bool multipleAlignmentFlag;
    uint32_t multipleSignatureBase=0;
    long seekPosition = ftell(inputFile);


    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    bool datasetGroupIdSuccessfulRead=readBytes(&inputBitstream,1,(char*)&datasetGroupId);
    bool datasetIdSuccessfulRead= readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16,
                                                                                 (char *) &datasetId);
    bool versionSuccessfulRead= readBytes(&inputBitstream, 4,(char *) &version);
    uint8_t value;
    bool unmappedIndexingFlagSuccessfulRead = readBit(&inputBitstream, &value);
    unmapped_indexing_flag = value!=0;
    bool byteOffsetSizeFlagSuccessfulRead = readBit(&inputBitstream, &value);
    byteOffsetSizeFlag = value!=0;
    bool nonOverlappingAURangeSuccessfulRead = readBit(&inputBitstream, &value);
    nonOverlappingAURange = value!=0;
    bool posOffsetIsUint40SuccessfulRead = readBit(&inputBitstream, &value);
    posOffsetIsUint40 = value!=0;
    bool blockHeaderFlagSuccessfulRead = readBit(&inputBitstream, &value);
    blockHeaderFlag = value!=0;

    if(
        !datasetGroupIdSuccessfulRead ||
        !datasetIdSuccessfulRead ||
        !versionSuccessfulRead ||
        !unmappedIndexingFlagSuccessfulRead ||
        !byteOffsetSizeFlagSuccessfulRead ||
        !posOffsetIsUint40SuccessfulRead ||
        !nonOverlappingAURangeSuccessfulRead ||
        !blockHeaderFlagSuccessfulRead
    ){
        fprintf(stderr,"Error reading datasetHeader.\n");
        return NULL;
    }
    if(blockHeaderFlag){
        bool mitFlagSuccessfulRead = readBit(&inputBitstream, &value);
        mitFlag = value!=0;
        bool classContiguousModeFlagSuccessfulRead = readBit(&inputBitstream, &value);
        classContiguousModeFlag = value!=0;
    }else{
        bool orderedBlocksFlagSuccessfulRead = readBit(&inputBitstream, &value);
        orderedBlocksFlag = value != 0;
    }

    bool sequencesCountSuccessfulRead= readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16,
                                                                                      (char *) &sequencesCount);
    bool referenceIdSuccessfulRead=readBytes(&inputBitstream,1,(char*)&referenceId);
    if(sequencesCountSuccessfulRead){
        seqIds = (uint16_t*)malloc(sequencesCount*sizeof(uint16_t));
        seqBlocks = (uint32_t*)malloc(sequencesCount*sizeof(uint32_t));

        if(seqIds==NULL || seqBlocks==NULL){
            fprintf(stderr,"Error reading dataset header: could not allocate memory.\n");
            free(seqIds);
            free(seqBlocks);
            return NULL;
        }
    }

    bool errorReading = false;
    for(typeof(sequencesCount) seq_i=0; seq_i<sequencesCount; seq_i++){
        if(!readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 16, (char *) (seqIds + seq_i))){
            errorReading = true;
            break;
        };
    }
    if (errorReading){
        fprintf(stderr,"Error reading dataset header.\n");
        free(seqIds);
        free(seqBlocks);
        return NULL;
    }
    for(typeof(sequencesCount) seq_i=0; seq_i<sequencesCount; seq_i++){
        if(!readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char *) (seqBlocks + seq_i))){
            errorReading = true;
            break;
        };
    }
    if (errorReading){
        fprintf(stderr,"Error reading dataset header.\n");
        free(seqIds);
        free(seqBlocks);
        return NULL;
    }

    //bool datasetTypeSuccessfulRead = true;
    bool datasetTypeSuccessfulRead = readNBitsShift(&inputBitstream,4,(char*)&datasetType);
    bool numClassesSuccessfulRead = readNBitsShift(&inputBitstream,4,(char*)&numClasses);

    if(!datasetTypeSuccessfulRead || !numClassesSuccessfulRead){
        fprintf(stderr,"Error reading dataset header.\n");
        free(seqIds);
        free(seqBlocks);
        return NULL;
    }

    classId = (uint8_t*)malloc(numClasses*sizeof(uint8_t));
    descriptorId = (uint8_t**)calloc(numClasses,sizeof(uint8_t*));
    if(!classId || !descriptorId){
        fprintf(stderr,"Error reading dataset header.\n");
        free(seqIds);
        free(seqBlocks);
        free(classId);
        free(descriptorId);
        return NULL;
    }

    numDescriptors = calloc(numClasses, sizeof(uint8_t));
    for(typeof(numClasses) class_i=0; class_i<numClasses; class_i++) {
        bool classIdSuccessfulRead = readNBitsShift(&inputBitstream, 4, (char *) classId + class_i);
        bool numDescriptorsSuccessfulRead = readNBitsShift(&inputBitstream, 5, (char *) numDescriptors + class_i);

        if (!classIdSuccessfulRead || !numDescriptorsSuccessfulRead) {
            fprintf(stderr, "Error reading dataset header.\n");
            free(seqIds);
            free(seqBlocks);
            free(classId);
            free(descriptorId);
            return NULL;
        }

        descriptorId[class_i] = (uint8_t *) malloc(sizeof(uint8_t) * numDescriptors[class_i]);
        for (uint8_t descId_i = 0; descId_i < numDescriptors[class_i]; descId_i++) {
            bool descriptorIdSuccessfulRead = readNBitsShift(&inputBitstream, 7, (char *) descriptorId[class_i] + descId_i);
            if (!descriptorIdSuccessfulRead) {
                fprintf(stderr, "Error reading dataset header.\n");
                free(seqIds);
                free(seqBlocks);
                free(classId);
                free(descriptorId);
                return NULL;
            }
        }
    }

    bool alphabetIdSuccessfulRead = readBytes(&inputBitstream,1,(char*)&alphabetId);
    bool numUClustersSuccessfulRead =
            readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32, (char *) &numUClusters);
    bool uSignatureConstantLengthSuccessfulRead = readBit(&inputBitstream,&value);
    uSignatureConstantLength = value!=0;

    readBytes(&inputBitstream,1,(char*)&uSignatureSize);

    if (!alphabetIdSuccessfulRead || !numUClustersSuccessfulRead || !uSignatureConstantLengthSuccessfulRead){
        fprintf(stderr,"Error reading dataset header.\n");
        free(seqIds);
        free(seqBlocks);
        free(classId);
        free(descriptorId);
        return NULL;
    }

    if(uSignatureConstantLength){
        if(!readBytes(&inputBitstream, 1, (char*)&uSignatureLength)){
            fprintf(stderr,"Error reading dataset header.\n");
            free(seqIds);
            free(seqBlocks);
            free(classId);
            free(descriptorId);
            return NULL;
        }
    }else{
        uSignatureLength = 0;
    }

    bool numUAccessUnitsSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream,32,(char*)&numberUAccessUnits);
    bool multipleAlignmentFlagSuccessfulRead = readBit(&inputBitstream, &value);
    multipleAlignmentFlag = value!=0;
    bool multipleSignatureBaseSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 31, (char*)&multipleSignatureBase);

    if(
        !numUAccessUnitsSuccessfulRead ||
        !multipleAlignmentFlagSuccessfulRead ||
        !multipleSignatureBaseSuccessfulRead
    ){
        fprintf(stderr,"Error reading dataset header.\n");
        free(seqIds);
        free(seqBlocks);
        free(classId);
        free(descriptorId);
        return NULL;
    }

    DatasetHeader* datasetHeader = initDatasetHeader(datasetGroupId, datasetId,
                                                     version, unmapped_indexing_flag, byteOffsetSizeFlag,
                                                     posOffsetIsUint40, nonOverlappingAURange, blockHeaderFlag,
                                                     sequencesCount, referenceId, datasetType, numClasses, alphabetId,
                                                     numUClusters, uSignatureConstantLength, uSignatureSize,
                                                     uSignatureLength, numberUAccessUnits, multipleAlignmentFlag,
                                                     multipleSignatureBase);


    if(datasetHeader == NULL){
        return datasetHeader;
    }
    datasetHeader->seekPosition = seekPosition;


    datasetHeader->mitFlag = mitFlag;
    datasetHeader->classContiguosModeFlag = classContiguousModeFlag;
    datasetHeader->blockStartCodePrefixFlag = blockStartCodePrefixFlag;
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

    uint8_t thresholdFlag;
    uint32_t currentThreshold;
    bool thresholdFlagSuccessfulRead = readBit(&inputBitstream,&thresholdFlag);
    bool currentThresholdSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 31,
                                                                                         (char *) &currentThreshold);

    if(!thresholdFlagSuccessfulRead || !currentThresholdSuccessfulRead){
        fprintf(stderr,"error reading threshold information.\n");
        freeDatasetHeader(datasetHeader);
        return NULL;
    }
    if(thresholdFlag==0){
        fprintf(stderr,"First threshold flag has to be 1.\n");
        freeDatasetHeader(datasetHeader);
        return NULL;
    }

    setThresholdForSequence(datasetHeader, 0,currentThreshold);
    for(uint16_t sequenceId=1; sequenceId<sequencesCount; sequenceId++){

        thresholdFlagSuccessfulRead = readBit(&inputBitstream,&thresholdFlag);
        if(thresholdFlag!=0) {
            currentThresholdSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(
                    &inputBitstream, 31, (char *) &currentThreshold
            );
        }

        if(!thresholdFlagSuccessfulRead || !currentThresholdSuccessfulRead){
            fprintf(stderr,"error reading threshold information.\n");
            freeDatasetHeader(datasetHeader);
            return NULL;
        }
        setThresholdForSequence(datasetHeader, sequenceId,currentThreshold);
    }
    forwardUntilAligned(&inputBitstream);
    return datasetHeader;
}

void setDatasetType(DatasetHeader* datasetHeader, uint8_t datasetType) {
    datasetHeader->datasetType = datasetType;
}

bool setNumberDescriptorsInClass(DatasetHeader* datasetHeader, unsigned int classId, uint8_t numberDescriptors) {
    if (datasetHeader->numClasses <= classId) return false;
    if (datasetHeader->numDescriptors == NULL) return false;
    datasetHeader->numDescriptors[classId] = numberDescriptors;

    datasetHeader->descriptorId[classId] = (uint8_t*)calloc(numberDescriptors,sizeof(uint8_t));


    return true;
}

void setClassType(DatasetHeader* datasetHeader, uint8_t classIt, uint8_t classType){
    datasetHeader->classId[classIt] = classType;
}

uint8_t getNumberDescriptorsInClass(DatasetHeader* datasetHeader, uint8_t class_index){
    if (datasetHeader->numClasses <= class_index) return 0;
    if (datasetHeader->numDescriptors == NULL) return 0;
    return datasetHeader->numDescriptors[class_index];
}

void setDescriptorIdInClass(DatasetHeader* datasetHeader, uint8_t classId, uint8_t descriptorIndex, uint8_t descriptorId){
    if (datasetHeader->numClasses <= classId) return;
    if (datasetHeader->numDescriptors == NULL) return;
    if (datasetHeader->descriptorId == NULL) return;
    if (descriptorIndex >= datasetHeader->numDescriptors[classId]) return;
    datasetHeader->descriptorId[classId][descriptorIndex] = descriptorId;
}

uint8_t getDescriptorIdInClass(DatasetHeader* datasetHeader, uint8_t classId, uint8_t descriptorIndex){
    if (datasetHeader->numClasses <= classId) return 0;
    if (datasetHeader->numDescriptors == NULL) return 0;
    if (datasetHeader->descriptorId == NULL) return 0;
    if (descriptorIndex >= datasetHeader->numDescriptors[classId]) return 0;
    return datasetHeader->descriptorId[classId][descriptorIndex];
}

void setConstantSignatureLength(DatasetHeader *datasetHeader, uint8_t uSignatureLength) {
    datasetHeader->uSignatureConstantLength=true;
    datasetHeader->uSignatureLength = uSignatureLength;
}

uint32_t getSignatureLength(DatasetHeader* datasetHeader) {
    return datasetHeader->uSignatureLength;
}

uint16_t getSequencesCount(DatasetHeader* datasetHeader) {
    return datasetHeader->sequencesCount;
}

uint16_t getSequenceId(DatasetHeader* datasetHeader, uint16_t sequence_index){
    return datasetHeader->seqIds[sequence_index];
}

uint8_t getClassesCount(DatasetHeader *datasetHeader) {
    return datasetHeader->numClasses;
}

bool hasClassType(DatasetHeader* datasetHeader, uint8_t classType){
    for(uint16_t class_i=0; class_i<datasetHeader->numClasses; class_i++){
        if (datasetHeader->classId[class_i]==classType){
            return true;
        }
    }
    return false;
}

uint8_t getClassType(DatasetHeader *datasetHeader, uint16_t class_i){
    return datasetHeader->classId[class_i];
}

bool getClassIndexForType(DatasetHeader *datasetHeader, uint8_t classType, uint8_t *class_i) {
    for(uint8_t class_it=0; class_it<datasetHeader->numClasses;class_it++){
        if (datasetHeader->classId[class_it]==classType){
            *class_i = class_it;
            return true;
        }
    }
    return false;
}

bool getDescriptorIndexForType(
    DatasetHeader *datasetHeader,
    uint8_t classType,
    uint8_t descriptorId,
    uint8_t *descriptorIndex
){
    uint8_t numberDescriptors = getNumberDescriptorsInClass(datasetHeader, classType);
    for(uint8_t descriptor_index=0; descriptor_index<numberDescriptors; descriptor_index++){
        if(datasetHeader->descriptorId[classType][descriptor_index]==descriptorId){
            *descriptorIndex = descriptor_index;
            return true;
        }
    }
    return false;
}

uint32_t getBlocksInSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex) {
    if(sequenceIndex>=datasetHeader->sequencesCount){
        return 0;
    }
    return datasetHeader->seqBlocks[sequenceIndex];

}

uint32_t getNumberUAccessUnits(DatasetHeader* datasetHeader) {
    return datasetHeader->numberUAccessUnits;
}

uint32_t getUSignatureSize(DatasetHeader* datasetHeader) {
    return datasetHeader->uSignatureSize;
}

uint32_t getMultipleSignatureBase(DatasetHeader* datasetHeader) {
    return datasetHeader->multipleSignatureBase;
}

void setMultipleSignatureBase(DatasetHeader* datasetHeader, uint32_t newMultipleSignatureBase) {
    datasetHeader->multipleSignatureBase = newMultipleSignatureBase;
}

long getDatasetHeaderSeekPosition(DatasetHeader* datasetHeader){
    return datasetHeader->seekPosition;
}

bool isBlockStartCodePrefixFlagSet(DatasetHeader* datasetHeader){
    return datasetHeader->blockStartCodePrefixFlag;
}

bool isMITFlagSet(DatasetHeader* datasetHeader){
    if (isBlockHeaderFlagSet(datasetHeader)){
        return datasetHeader->mitFlag;
    }else{
        return true;
    }
}

void setMITFlag(DatasetHeader* datasetHeader, bool value){
    datasetHeader->mitFlag = value;
}