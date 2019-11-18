//
// Created by gencom on 4/12/17.
//

#include <memory.h>
#include "../../Boxes.h"
#include "../../utils.h"

AccessUnitContainer* initAccessUnitContainer(DatasetContainer* datasetContainer) {
    AccessUnitContainer* accessUnitContainer = malloc(sizeof(AccessUnitContainer));
    if (accessUnitContainer != NULL) {
        accessUnitContainer->accessUnitHeader = NULL;
        accessUnitContainer->accessUnitInformation = NULL;
        accessUnitContainer->accessUnitProtection = NULL;
        accessUnitContainer->seekPosition = -1;
        accessUnitContainer->blocks = NULL;
        accessUnitContainer->datasetContainer = datasetContainer;
    }
    return accessUnitContainer;
}

void freeAccessUnitContainer(AccessUnitContainer* accessUnitContainer) {
    if (accessUnitContainer->blocks != NULL && accessUnitContainer->accessUnitHeader) {
        for (uint8_t block_i = 0; block_i < getNumBlocks(accessUnitContainer->accessUnitHeader); block_i++) {
            Block* block = getValue(accessUnitContainer->blocks, block_i);
            if (block != NULL) {
                freeBlock(block);
            }
        }
        freeVector(accessUnitContainer->blocks);
    }
    if (accessUnitContainer->accessUnitHeader != NULL) {
        freeAccessUnitHeader(accessUnitContainer->accessUnitHeader);
    }
    if (accessUnitContainer->accessUnitInformation != NULL) {
        freeAccessUnitInformation(accessUnitContainer->accessUnitInformation);
    }
    if (accessUnitContainer->accessUnitProtection != NULL) {
        freeAccessUnitProtection(accessUnitContainer->accessUnitProtection);
    }
    free(accessUnitContainer);
}

bool isAccessUnitContainerValid(AccessUnitContainer* accessUnitContainer) {
    return accessUnitContainer->accessUnitHeader != NULL;
}

uint64_t getAccessUnitContainerContentSize(AccessUnitContainer* accessUnitContainer) {
    uint64_t contentSize = 0;
    if (accessUnitContainer->accessUnitHeader != NULL) {
        contentSize += getAccessUnitHeaderSize(accessUnitContainer->accessUnitHeader);
    }
    if (accessUnitContainer->blocks != NULL && accessUnitContainer->accessUnitHeader != NULL) {
        for (uint8_t block_i = 0; block_i < getNumBlocks(accessUnitContainer->accessUnitHeader); block_i++) {
            Block* block = getValue(accessUnitContainer->blocks, block_i);
            if (block != NULL) {
                contentSize += getBlockSize(block);
            }
        }
    }
    if (accessUnitContainer->accessUnitInformation != NULL) {
        contentSize += getSizeAccessUnitInformation(accessUnitContainer->accessUnitInformation);
    }
    if (accessUnitContainer->accessUnitProtection != NULL) {
        contentSize += getSizeAccessUnitProtection(accessUnitContainer->accessUnitProtection);
    }
    // contentSize += 12;
    return contentSize;
}

uint64_t getAccessUnitContainerSize(AccessUnitContainer* accessUnitContainer) {
    return BOX_HEADER_SIZE + getAccessUnitContainerContentSize(accessUnitContainer);
}

bool writeAccessUnitContainer(FILE* outputFile, AccessUnitContainer* accessUnitContainer) {
    uint64_t contentSize = getAccessUnitContainerSize(accessUnitContainer);
    writeBoxHeader(outputFile, accessUnitContainerName, contentSize);

    if (accessUnitContainer->accessUnitHeader != NULL) {
        if (!writeAccessUnitHeader(outputFile, accessUnitContainer->accessUnitHeader)) {
            fprintf(stderr, "Error writing access unit container's header.\n");
            return false;
        };
    }

    if (isBlockHeaderFlagSet(getDatasetHeader(accessUnitContainer->datasetContainer))) {
        size_t numBlocks = getSize(accessUnitContainer->blocks);
        for (size_t block_i = 0; block_i < numBlocks; block_i++) {
            Block* block = getValue(accessUnitContainer->blocks, block_i);
            if (block == NULL) {
                fprintf(stderr, "Error in memory initialization: block pointer null.\n");
                return false;
            } else {
                if (!writeBlock(block, outputFile)) {
                    fprintf(stderr, "Error writing block.\n");
                    return false;
                }
            }
        }
    }

    if (accessUnitContainer->accessUnitInformation != NULL) {
        if (!writeAccessUnitInformation(outputFile, accessUnitContainer->accessUnitInformation)) {
            fprintf(stderr, "Error writing access unit container's information.\n");
            return false;
        };
    }
    if (accessUnitContainer->accessUnitProtection != NULL) {
        if (!writeAccessUnitProtection(accessUnitContainer->accessUnitProtection, outputFile)) {
            fprintf(stderr, "Error writing access unit container's information.\n");
            return false;
        };
    }
    return true;
}

AccessUnitHeader* getAccessUnitHeader(AccessUnitContainer* accessUnitContainer) {
    return accessUnitContainer->accessUnitHeader;
}

void setAccessUnitContainerHeader(AccessUnitContainer* accessUnitContainer, AccessUnitHeader* accessUnitHeader) {
    if (accessUnitContainer->accessUnitHeader != NULL) {
        freeAccessUnitHeader(accessUnitContainer->accessUnitHeader);
    }
    accessUnitContainer->accessUnitHeader = accessUnitHeader;
}

void setAccessUnitContainerProtection(AccessUnitContainer* accessUnitContainer,
                                      AccessUnitProtection* accessUnitProtection) {
    if (accessUnitContainer->accessUnitProtection != NULL) {
        freeAccessUnitProtection(accessUnitContainer->accessUnitProtection);
    }
    accessUnitContainer->accessUnitProtection = accessUnitProtection;
}

void setAccessUnitContainerInformation(AccessUnitContainer* accessUnitContainer,
                                       AccessUnitInformation* accessUnitInformation) {
    if (accessUnitContainer->accessUnitInformation != NULL) {
        freeAccessUnitInformation(accessUnitContainer->accessUnitInformation);
    }
    accessUnitContainer->accessUnitInformation = accessUnitInformation;
}

AccessUnitContainer* parseAccessUnitContainer(uint64_t boxContentSize, FILE* inputFile, char* fileName,
                                              DatasetContainer* datasetContainer) {
    char type[5];
    uint64_t boxSize;
    AccessUnitContainer* accessUnitContainer = initAccessUnitContainer(datasetContainer);
    if (accessUnitContainer == NULL) {
        fprintf(stderr, "Access unit container could not be allocated.\n");
        return accessUnitContainer;
    }
    accessUnitContainer->seekPosition = ftell(inputFile);

    enum PreviousState { init, header, information, protection } previousState = init;

    while (getAccessUnitContainerContentSize(accessUnitContainer) < boxContentSize) {
        if (!readBoxHeader(inputFile, type, &boxSize)) {
            fprintf(stderr, "Access unit container content could not be read.\n");
            freeAccessUnitContainer(accessUnitContainer);
            return NULL;
        }
        if (strncmp(type, accessUnitHeaderName, 4) == 0) {
            if (previousState != init) {
                fprintf(stderr, "Access unit container wrong order of components.\n");
                return NULL;
            }
            previousState = header;
            AccessUnitHeader* accessUnitHeader = parseAccessUnitHeader(inputFile, datasetContainer);
            if (accessUnitHeader == NULL) {
                freeAccessUnitContainer(accessUnitContainer);
                return NULL;
            }
            setAccessUnitContainerHeader(accessUnitContainer, accessUnitHeader);

        } else if (strncmp(type, accessUnitProtectionName, 4) == 0) {
            if (!(previousState == header || previousState == protection)) {
                fprintf(stderr, "Access unit container wrong order of components.\n");
                return NULL;
            }
            previousState = protection;

            AccessUnitProtection* accessUnitProtection = parseAccessUnitProtection(boxSize, inputFile);
            if (accessUnitProtection == NULL) {
                freeAccessUnitContainer(accessUnitContainer);
                return NULL;
            }
            setAccessUnitContainerProtection(accessUnitContainer, accessUnitProtection);
        } else if (strncmp(type, accessUnitInformationName, 4) == 0) {
            if (!(previousState == header)) {
                fprintf(stderr, "Access unit container wrong order of components.\n");
                return NULL;
            }
            previousState = information;

            AccessUnitInformation* accessUnitInformation = parseAccessUnitInformation(boxSize, inputFile);
            if (accessUnitInformation == NULL) {
                freeAccessUnitContainer(accessUnitContainer);
                return NULL;
            }
            setAccessUnitContainerInformation(accessUnitContainer, accessUnitInformation);
        } else {
            fseek(inputFile, -BOX_HEADER_SIZE, SEEK_CUR);
            if (isBlockHeaderFlagSet(getDatasetHeader(datasetContainer)) && previousState != init) {
                uint8_t numBlocks = getNumBlocks(accessUnitContainer->accessUnitHeader);
                for (uint8_t block_i = 0; block_i < numBlocks; block_i++) {
                    long blockStartingPosition = ftell(inputFile);
                    Block* block = parseBlockContainerAUCmode(datasetContainer, inputFile, fileName);
                    if (block == NULL) {
                        freeAccessUnitContainer(accessUnitContainer);
                        return NULL;
                    }
                    addBlock(accessUnitContainer, block);
                }
            } else {
                fprintf(stderr, "Error: unknown access unit container element.\n");
                return NULL;
            }
        }
    }
    return accessUnitContainer;
}

long getAccessUnitContainerSeekPoint(AccessUnitContainer* accessUnitContainer) {
    return accessUnitContainer->seekPosition;
}

void addBlock(AccessUnitContainer* accessUnitContainer, Block* block) {
    if (accessUnitContainer->blocks == NULL) {
        accessUnitContainer->blocks = initVector();
    }
    pushBack(accessUnitContainer->blocks, block);
}

Vector* getBlocks(AccessUnitContainer* accessUnitContainer) { return accessUnitContainer->blocks; }