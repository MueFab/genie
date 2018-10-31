#include <stdlib.h>
#include <Boxes.h>
#include "../../Boxes.h"//
// Created by gencom on 26/07/17.
//
#include "../../utils.h"
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"

StreamHeader* initStreamHeader(
    StreamContainer* streamContainer,
    uint8_t descriptorId,
    uint8_t classId,
    uint16_t parametersSetId,
    uint32_t numberOfBlocks,
    uint8_t protectionFlag
){
    StreamHeader* streamHeader = (StreamHeader*)malloc(sizeof(StreamHeader));
    if (streamHeader == NULL){
        return NULL;
    }

    streamHeader->streamContainer = streamContainer;
    streamHeader->descriptorId = descriptorId;
    streamHeader->classId = classId;
    streamHeader->parametersSetId = parametersSetId;
    streamHeader->numberOfBlocks = numberOfBlocks;
    streamHeader->protectionFlag = protectionFlag;
    streamHeader->seekPosition = -1;

    return streamHeader;
}

uint8_t getStreamClass(StreamHeader* streamHeader){
    return streamHeader->classId;
}

uint8_t getStreamDescriptorId(StreamHeader* streamHeader){
    return streamHeader->descriptorId;
}

bool writeContentStreamHeader(StreamHeader* streamHeader, FILE *outputFile){
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    bool descriptorIdSuccessfulWrite = writeNBitsShift(&outputBitstream, 7, (char *) &(streamHeader->descriptorId));
    bool classIdSuccessfulWrite = writeNBitsShift(&outputBitstream, 4, (char *) &streamHeader->classId);
    bool parametersSetIdSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian16(&outputBitstream, 13,
                                                                                    streamHeader->parametersSetId);
    bool numberOfBlocksSuccessfulWrite = writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, streamHeader->numberOfBlocks);
    bool protectionFlagSuccessfulWrite = writeBytes(&outputBitstream,1,(char*)&streamHeader->protectionFlag);
    writeBuffer(&outputBitstream);

    if (
        !descriptorIdSuccessfulWrite ||
        !classIdSuccessfulWrite ||
        !parametersSetIdSuccessfulWrite ||
        !numberOfBlocksSuccessfulWrite ||
        !protectionFlagSuccessfulWrite
    ){
        fprintf(stderr,"Error writing stream header.\n");
        return false;
    }
    return true;
}

bool writeStreamHeader(StreamHeader* streamHeader, FILE *outputFile){
    bool typeSuccessfulWrite = fwrite(streamHeaderName,sizeof(char),4,outputFile)==4;
    uint64_t sizeStreamHeader = getSizeStreamHeader();
    uint64_t sizeStreamHeaderBigEndian = nativeToBigEndian64(sizeStreamHeader);
    bool sizeSuccessfulWrite = fwrite(&sizeStreamHeaderBigEndian, sizeof(uint64_t),1,outputFile)==1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite){
        fprintf(stderr,"Error writing type or size of stream header.\n");
        return false;
    }
    return writeContentStreamHeader(streamHeader, outputFile);
}

uint64_t getSizeContentStreamHeader() {
    return 8;
}

uint64_t getSizeStreamHeader() {
    return BOX_HEADER_SIZE + getSizeContentStreamHeader();
}

StreamHeader *parseStreamHeader(StreamContainer* streamContainer, FILE *inputFile){
    uint8_t descriptorId;
    uint8_t classId;
    uint16_t parametersSetId;
    uint32_t numberOfBlocks;
    uint8_t protectionFlag;

    long seekPosition = ftell(inputFile);

    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    bool descriptorIdSuccessfulRead =readNBitsShift(&inputBitstream,7,(char*)&descriptorId);
    bool classIdSuccessfulRead = readNBitsShift(&inputBitstream,4,(char*)&classId);
    bool parametersSetIdSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian16(&inputBitstream, 13,
                                                                                        (char *) &parametersSetId);
    bool numberOfBlocksSuccessfulRead = readNBitsShiftAndConvertBigToNativeEndian32(&inputBitstream, 32,
                                                                                       (char *) &numberOfBlocks);
    bool protectionFlagSuccessfulRead = readBytes(&inputBitstream,1,(char*)&protectionFlag);

    if (
        !descriptorIdSuccessfulRead ||
        !classIdSuccessfulRead ||
        !parametersSetIdSuccessfulRead ||
        !numberOfBlocksSuccessfulRead ||
        !protectionFlagSuccessfulRead
    ){
        fprintf(stderr,"Error reading stream header.\n");
        return NULL;
    }

    StreamHeader* streamHeader =
            initStreamHeader(streamContainer,descriptorId,classId,parametersSetId,numberOfBlocks,protectionFlag);
    streamHeader->seekPosition = seekPosition;
    return streamHeader;

}

void freeStreamHeader(StreamHeader* streamHeader){
    free(streamHeader);
}

long getStreamHeaderSeekPosition(StreamHeader* streamHeader){
    return streamHeader->seekPosition;
}