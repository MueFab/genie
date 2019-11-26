//
// Created by gencom on 26/09/17.
//

#include "FromFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FromFile* initFromFileWithFilename(char* filename) {
    FromFile* fromFile = (FromFile*)malloc(sizeof(FromFile));
    if (fromFile == NULL) {
        return NULL;
    }
    fromFile->initiated = false;
    fromFile->filename = (char*)calloc(sizeof(char), strlen(filename) + 1);
    strncpy(fromFile->filename, filename, strlen(filename));
    fromFile->startPos = 0;

    FILE* openedFile = fopen(filename, "r");
    if (openedFile != NULL) {
        fseek(openedFile, 0, SEEK_END);
        long endPosRead = ftell(openedFile);
        if (endPosRead == -1) {
            fprintf(stderr, "Could not get file position.\n");
        }
        fromFile->endPos = (uint64_t)endPosRead;
        fclose(openedFile);
        fromFile->initiated = true;
    }
    return fromFile;
}

FromFile* initFromFile() {
    FromFile* fromFile = (FromFile*)malloc(sizeof(FromFile));
    if (fromFile == NULL) {
        return NULL;
    }
    fromFile->filename = NULL;
    fromFile->startPos = 0;
    fromFile->endPos = 0;
    fromFile->initiated = false;

    return fromFile;
}

FromFile* initFromFileWithFilenameAndBoundaries(char* filename, uint64_t startPos, uint64_t endPos) {
    FromFile* fromFile = (FromFile*)malloc(sizeof(FromFile));
    if (fromFile == NULL) {
        return NULL;
    }
    fromFile->filename = (char*)calloc(sizeof(char), strlen(filename) + 1);
    strncpy(fromFile->filename, filename, strlen(filename));

    fromFile->startPos = startPos;
    fromFile->endPos = endPos;
    fromFile->initiated = true;

    return fromFile;
}

u_int64_t getFromFileSize(FromFile* fromFile) { return fromFile->endPos - fromFile->startPos; }

bool writeFromFile(FromFile* fromFile, FILE* outputFile) {
    if (isFromFileInitiated(fromFile)) {
        FILE* openedFile = fopen(fromFile->filename, "r");
        fseek(openedFile, (long)fromFile->startPos, SEEK_SET);
        uint64_t size = getFromFileSize(fromFile);
        uint64_t remainingSize = size;
        char buffer[1024];
        while (remainingSize > 0) {
            uint64_t toRead = (uint64_t)(remainingSize > 1024 ? 1024 : remainingSize);
            if (toRead != fread(buffer, 1, toRead * sizeof(char), openedFile)) {
                fclose(openedFile);
                return false;
            };
            if (toRead != fwrite(buffer, 1, toRead * sizeof(char), outputFile)) {
                fclose(openedFile);
                return false;
            };
            remainingSize -= toRead;
        }
        fclose(openedFile);
        return true;
    }
    return false;
}

bool isFromFileInitiated(FromFile* fromFile) { return fromFile->initiated; }

void freeFromFile(FromFile* fromFile) {
    free(fromFile->filename);
    free(fromFile);
}

FromFile* cloneFromFile(FromFile* originalFromFile) {
    FromFile* fromFile = (FromFile*)malloc(sizeof(FromFile));
    if (fromFile == NULL) {
        return NULL;
    }
    fromFile->initiated = originalFromFile->initiated;
    fromFile->startPos = originalFromFile->startPos;
    fromFile->endPos = originalFromFile->endPos;
    fromFile->filename = (char*)calloc(sizeof(char), strlen(originalFromFile->filename) + 1);
    strncpy(fromFile->filename, originalFromFile->filename, strlen(originalFromFile->filename));

    return fromFile;
}
