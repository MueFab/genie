//
// Created by gencom on 27/07/17.
//

#include <stdlib.h>
#include <string.h>
#include "../../Boxes.h"
#include "../../DataStructures/FromFile.h"
#include "../../utils.h"

StreamContainer* initStreamContainer() {
    StreamContainer* streamContainer = (StreamContainer*)calloc(1, sizeof(StreamContainer));
    streamContainer->streamHeader = NULL;
    streamContainer->streamProtection = NULL;
    streamContainer->datasFromFile = initVector();
    streamContainer->hasSeek = false;
    streamContainer->seekPosition = 0;
    return streamContainer;
}

void freeStreamContainer(StreamContainer* streamContainer) {
    if (streamContainer->streamHeader != NULL) {
        freeStreamHeader(streamContainer->streamHeader);
    }
    if (streamContainer->streamProtection != NULL) {
        freeStreamProtection(streamContainer->streamProtection);
    }
    if (streamContainer->datasFromFile != NULL) {
        size_t numberBlocksInStream = getSize(streamContainer->datasFromFile);
        for (size_t block_i = 0; block_i < numberBlocksInStream; block_i++) {
            FromFile* fromFileBlock = getValue(streamContainer->datasFromFile, block_i);
            freeFromFile(fromFileBlock);
        }
        freeVector(streamContainer->datasFromFile);
    }
    free(streamContainer);
}

uint64_t getSizeContentStreamContainer(StreamContainer* streamContainer) {
    uint64_t contentSize = 0;
    if (streamContainer->streamHeader != NULL) {
        contentSize += getSizeStreamHeader();
    }
    if (streamContainer->streamProtection != NULL) {
        contentSize += getSizeStreamProtection(streamContainer->streamProtection);
    }
    if (streamContainer->datasFromFile != NULL) {
        size_t numberBlocksInStream = getSize(streamContainer->datasFromFile);
        for (size_t block_i = 0; block_i < numberBlocksInStream; block_i++) {
            FromFile* fromFileBlock = getValue(streamContainer->datasFromFile, block_i);
            contentSize += getFromFileSize(fromFileBlock);
        }
    }

    return contentSize;
}

uint64_t getSizeStreamContainer(StreamContainer* streamContainer) {
    return BOX_HEADER_SIZE + getSizeContentStreamContainer(streamContainer);
}

bool writeContentStreamContainer(StreamContainer* streamContainer, FILE* outputFile) {
    if (streamContainer->streamHeader != NULL) {
        if (!writeStreamHeader(streamContainer->streamHeader, outputFile)) {
            fprintf(stderr, "Error writing stream header.\n");
            return false;
        }
    }
    if (streamContainer->streamProtection != NULL) {
        if (!writeStreamProtection(streamContainer->streamProtection, outputFile)) {
            fprintf(stderr, "Error writing stream protection.\n");
            return false;
        }
    }
    if (streamContainer->datasFromFile != NULL) {
        size_t numberBlocksInStream = getSize(streamContainer->datasFromFile);
        for (size_t block_i = 0; block_i < numberBlocksInStream; block_i++) {
            FromFile* fromFileBlock = getValue(streamContainer->datasFromFile, block_i);
            if (!writeFromFile(fromFileBlock, outputFile)) {
                fprintf(stderr, "Error writing stream content.\n");
            }
        }
    }

    return true;
}

bool writeStreamContainer(StreamContainer* streamContainer, FILE* outputFile) {
    bool typeSuccessfulWrite = fwrite(streamContainerName, sizeof(char), 4, outputFile) == 4;
    uint64_t sizeStreamContainer = getSizeStreamContainer(streamContainer);
    uint64_t sizeStreamContainerBigEndian = nativeToBigEndian64(sizeStreamContainer);
    bool sizeSuccessfulWrite = fwrite(&sizeStreamContainerBigEndian, sizeof(uint64_t), 1, outputFile) == 1;
    if (!typeSuccessfulWrite || !sizeSuccessfulWrite) {
        fprintf(stderr, "Error writing type and size of stream container.\n");
        return false;
    }
    return writeContentStreamContainer(streamContainer, outputFile);
}

bool isStreamContainerValid(StreamContainer* streamContainer) { return streamContainer->streamHeader != NULL; }

void setStreamHeader(StreamContainer* streamContainer, StreamHeader* streamHeader) {
    streamContainer->streamHeader = streamHeader;
}

void setStreamProtection(StreamContainer* streamContainer, StreamProtection* streamProtection) {
    streamContainer->streamProtection = streamProtection;
}

void setDataFromFilename(StreamContainer* streamContainer, char* filename) {
    if (streamContainer->datasFromFile != NULL) {
        size_t numberBlocksInStream = getSize(streamContainer->datasFromFile);
        for (size_t block_i = 0; block_i < numberBlocksInStream; block_i++) {
            FromFile* fromFileBlock = getValue(streamContainer->datasFromFile, block_i);
            freeFromFile(fromFileBlock);
        }
        freeVector(streamContainer->datasFromFile);
    }
    streamContainer->datasFromFile = initVector();
    setValue(streamContainer->datasFromFile, 0, initFromFileWithFilename(filename));
}

void setDataFromFilenameWithBoundaries(StreamContainer* streamContainer, char* filename, uint64_t startPos,
                                       uint64_t endPos) {
    if (streamContainer->datasFromFile != NULL) {
        size_t numberBlocksInStream = getSize(streamContainer->datasFromFile);
        for (size_t block_i = 0; block_i < numberBlocksInStream; block_i++) {
            FromFile* fromFileBlock = getValue(streamContainer->datasFromFile, block_i);
            freeFromFile(fromFileBlock);
        }
        freeVector(streamContainer->datasFromFile);
    }
    streamContainer->datasFromFile = initVector();
    setValue(streamContainer->datasFromFile, 0, initFromFileWithFilenameAndBoundaries(filename, startPos, endPos));
}

StreamContainer* parseStreamContainer(uint64_t boxContentSize, FILE* inputFile, char* fileName,
                                      DatasetContainer* datasetContainer) {
    StreamContainer* streamContainer = initStreamContainer();
    long seekPosition = ftell(inputFile);
    if (seekPosition == -1) {
        fprintf(stderr, "Could not get file position.\n");
        freeStreamContainer(streamContainer);
        return NULL;
    }
    streamContainer->seekPosition = (uint64_t)seekPosition;

    char type[5];
    uint64_t boxSize;
    uint64_t dataContentSize = boxContentSize;

    while (getSizeContentStreamContainer(streamContainer) < boxContentSize) {
        if (!readBoxHeader(inputFile, type, &boxSize)) {
            fprintf(stderr, "Stream container content could not be read.\n");
            freeStreamContainer(streamContainer);
            return NULL;
        }
        if (strncmp(type, streamHeaderName, 4) == 0) {
            StreamHeader* streamHeader = parseStreamHeader(streamContainer, inputFile);
            if (streamHeader != NULL) {
                setStreamHeader(streamContainer, streamHeader);
            } else {
                fprintf(stderr, "Error reading stream header.\n");
                freeStreamContainer(streamContainer);
                return NULL;
            }
            dataContentSize -= getSizeStreamHeader();
        } else if (strncmp(type, streamProtectionName, 4) == 0) {
            StreamProtection* streamProtection = parseStreamProtection(streamContainer, boxSize - 12, inputFile);
            if (streamProtection != NULL) {
                setStreamProtection(streamContainer, streamProtection);
            } else {
                fprintf(stderr, "Error reading stream protection.\n");
                freeStreamContainer(streamContainer);
                return NULL;
            }
            dataContentSize -= getSizeStreamProtection(streamProtection);
        } else {
            fseek(inputFile, -12, SEEK_CUR);
            long startPositionRead = ftell(inputFile);
            if (startPositionRead == -1) {
                fprintf(stderr, "Could not get file position.\n");
                return NULL;
            }
            uint64_t startPosition = (uint64_t)startPositionRead;

            uint64_t endPosition = startPosition + dataContentSize;
            setDataFromFilenameWithBoundaries(streamContainer, fileName, startPosition, endPosition);
            fseek(inputFile, (long)(endPosition - startPosition), SEEK_CUR);
            if (streamContainer->datasFromFile != NULL) {
                size_t numberBlocksInStream = getSize(streamContainer->datasFromFile);
                for (size_t block_i = 0; block_i < numberBlocksInStream; block_i++) {
                    FromFile* fromFileBlock = getValue(streamContainer->datasFromFile, block_i);
                    dataContentSize -= getFromFileSize(fromFileBlock);
                }
            }

            break;
        }
    }

    uint64_t sizeBlocks = 0;
    if (streamContainer->datasFromFile != NULL) {
        size_t numberBlocksInStream = getSize(streamContainer->datasFromFile);
        for (size_t block_i = 0; block_i < numberBlocksInStream; block_i++) {
            FromFile* fromFileBlock = getValue(streamContainer->datasFromFile, block_i);
            sizeBlocks += getFromFileSize(fromFileBlock);
        }
    }

    if (dataContentSize != 0 || sizeBlocks == 0) {
        fprintf(stderr, "Error reading stream container.\n");
        freeStreamContainer(streamContainer);
        return NULL;
    }

    if (datasetContainer != NULL && datasetContainer->datasetMasterIndexTable != NULL) {
        DatasetMasterIndexTable* datasetMasterIndexTable = datasetContainer->datasetMasterIndexTable;
        uint64_t finalOffset = (uint64_t)ftell(inputFile) - datasetContainer->seekPosition;

        ClassType streamClass = getStreamClass(streamContainer->streamHeader);
        uint8_t streamDescriptor = getStreamDescriptorId(streamContainer->streamHeader);

        uint8_t class_index;
        if (!getClassIndexForType(datasetContainer->datasetHeader, streamClass, &class_index)) {
            fprintf(stderr, "Error unknown class.\n");
            freeStreamContainer(streamContainer);
            return NULL;
        }

        uint8_t descriptor_index;
        if (!getDescriptorIndexForType(datasetContainer->datasetHeader, streamClass, streamDescriptor,
                                       &descriptor_index)) {
            fprintf(stderr, "Error unknown descriptor.\n");
            freeStreamContainer(streamContainer);
            return NULL;
        }

        if (datasetMasterIndexTable->offsetsPerClass != NULL) {
            insertFinalOffset(datasetMasterIndexTable, streamClass, class_index, descriptor_index, finalOffset);
        }
    }

    return streamContainer;
}

Vector* getDataFromStreamContainer(StreamContainer* streamContainer) { return streamContainer->datasFromFile; }

uint64_t getStreamContainerSeekPosition(StreamContainer* streamContainer) { return streamContainer->seekPosition; }

bool generateStreamSeekPoints(StreamContainer* streamContainer) {
    fprintf(stdout, "\tstream @ %li-%li\n", getStreamContainerSeekPosition(streamContainer),
            getStreamContainerSeekPosition(streamContainer) + getSizeContentStreamContainer(streamContainer));
    if (streamContainer->streamHeader != NULL) {
        fprintf(stdout, "\t\tstream header @ %li-%li\n", getStreamHeaderSeekPosition(streamContainer->streamHeader),
                getStreamHeaderSeekPosition(streamContainer->streamHeader) + getSizeContentStreamHeader());
    }
    if (streamContainer->streamProtection != NULL) {
        fprintf(stdout, "\t\tstream protection @ %li-%li\n",
                getStreamProtectionSeekPosition(streamContainer->streamProtection),
                getStreamProtectionSeekPosition(streamContainer->streamProtection) +
                    getSizeContentStreamProtection(streamContainer->streamProtection));
    }
    /*fprintf(
            stdout,
            "\t\tstream data @ %li-%li\n",
            streamContainer->dataFromFile->startPos,
            streamContainer->dataFromFile->endPos
    );*/
    return true;
}

bool addBlockToStream(StreamContainer* streamContainer, FromFile* block) {
    return pushBack(streamContainer->datasFromFile, block);
}