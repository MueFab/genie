#include <stdlib.h>
#include "../../Boxes.h"
// Created by gencom on 26/07/17.
//
#include "../../DataStructures/BitStreams/InputBitstream.h"
#include "../../DataStructures/BitStreams/OutputBitstream.h"
#include "../../utils.h"

StreamHeader* initStreamHeader(StreamContainer* streamContainer, uint8_t descriptorId, ClassType classId,
                               uint32_t numberOfBlocks) {
    StreamHeader* streamHeader = (StreamHeader*)malloc(sizeof(StreamHeader));
    if (streamHeader == NULL) {
        return NULL;
    }

    streamHeader->streamContainer = streamContainer;
    streamHeader->descriptorId = descriptorId;
    streamHeader->classId = classId;
    streamHeader->numberOfBlocks = numberOfBlocks;
    streamHeader->hasSeek = false;
    streamHeader->seekPosition = 0;

    return streamHeader;
}

ClassType getStreamClass(StreamHeader* streamHeader) { return streamHeader->classId; }

uint8_t getStreamDescriptorId(StreamHeader* streamHeader) { return streamHeader->descriptorId; }

bool writeContentStreamHeader(StreamHeader* streamHeader, FILE* outputFile) {
    OutputBitstream outputBitstream;
    initializeOutputBitstream(&outputBitstream, outputFile);

    bool descriptorIdSuccessfulWrite = writeBytes(&outputBitstream, 1, (char*)&(streamHeader->descriptorId));
    bool classIdSuccessfulWrite = writeNBitsShift(&outputBitstream, 4, (char*)&streamHeader->classId);
    bool numberOfBlocksSuccessfulWrite =
        writeNBitsShiftAndConvertToBigEndian32(&outputBitstream, 32, streamHeader->numberOfBlocks);
    writeBuffer(&outputBitstream);

    if (!descriptorIdSuccessfulWrite || !classIdSuccessfulWrite || !numberOfBlocksSuccessfulWrite) {
        fprintf(stderr, "Error writing stream header.\n");
        return false;
    }
    return true;
}

bool writeStreamHeader(StreamHeader* streamHeader, FILE* outputFile) {
    bool typeSuccessfulWrite = fwrite(streamHeaderName, sizeof(char), 4, outputFile) == 4;
    uint64_t sizeStreamHeader = getSizeStreamHeader();
    uint64_t sizeStreamHeaderBigEndian = nativeToBigEndian64(sizeStreamHeader);
    bool sizeSuccessfulWrite = fwrite(&sizeStreamHeaderBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error writing type or size of stream header.\n");
        return false;
    }
    return writeContentStreamHeader(streamHeader, outputFile);
}

uint64_t getSizeContentStreamHeader() { return 6; }

uint64_t getSizeStreamHeader() { return BOX_HEADER_SIZE + getSizeContentStreamHeader(); }

StreamHeader* parseStreamHeader(StreamContainer* streamContainer, FILE* inputFile) {
    uint8_t descriptorId;
    ClassType classId;
    uint32_t numberOfBlocks;

    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        return NULL;
    }

    InputBitstream inputBitstream;
    initializeInputBitstream(&inputBitstream, inputFile);

    bool descriptorIdSuccessfulRead = readBytes(&inputBitstream, 1, (char*)&descriptorId);
    uint8_t classTypeBuffer;
    bool classIdSuccessfulRead = readNBitsShift(&inputBitstream, 4, (char*)&classTypeBuffer);
    bool numberOfBlocksSuccessfulRead = readNBitsBigToNativeEndian32(&inputBitstream, 32, &numberOfBlocks);

    if (!descriptorIdSuccessfulRead || !classIdSuccessfulRead || !numberOfBlocksSuccessfulRead) {
        fprintf(stderr, "Error reading stream header.\n");
        return NULL;
    }

    classId.classType = classTypeBuffer;
    StreamHeader* streamHeader = initStreamHeader(streamContainer, descriptorId, classId, numberOfBlocks);
    streamHeader->seekPosition = (size_t)seekPosition;
    streamHeader->hasSeek = true;
    return streamHeader;
}

void freeStreamHeader(StreamHeader* streamHeader) { free(streamHeader); }

size_t getStreamHeaderSeekPosition(StreamHeader* streamHeader) { return streamHeader->seekPosition; }
