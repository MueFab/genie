//
// Created by gencom on 26/09/17.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "FromFile.h"

FromFile* initFromFileWithFilename(char* filename){
    FromFile* fromFile = (FromFile*)malloc(sizeof(FromFile));
    if(fromFile == NULL){
        return NULL;
    }
    fromFile->initiated = false;
    fromFile->filename = (char*)calloc(sizeof(char),strlen(filename)+1);
    strncpy(fromFile->filename, filename, strlen(filename));
    fromFile->startPos = 0;

    FILE* openedFile = fopen(filename,"r");
    if (openedFile != NULL) {
        fseek(openedFile, 0, SEEK_END);
        fromFile->endPos = ftell(openedFile);
        fclose(openedFile);
        fromFile->initiated = true;
    }
}

FromFile* initFromFile(){
    FromFile* fromFile = (FromFile*)malloc(sizeof(FromFile));
    if(fromFile == NULL){
        return NULL;
    }
    fromFile->filename = NULL;
    fromFile->startPos = 0;
    fromFile->endPos = 0;
    fromFile->initiated = false;

    return fromFile;
}

FromFile* initFromFileWithFilenameAndBoundaries(char *filename, long startPos, long endPos){
    FromFile* fromFile = (FromFile*)malloc(sizeof(FromFile));
    if(fromFile == NULL){
        return NULL;
    }
    fromFile->filename = (char*)calloc(sizeof(char), strlen(filename)+1);
    strncpy(fromFile->filename, filename, strlen(filename));

    fromFile->startPos = startPos;
    fromFile->endPos = endPos;
    fromFile->initiated = true;

    return fromFile;
}

long getFromFileSize(FromFile* fromFile){
    return fromFile->endPos-fromFile->startPos;
}

bool writeFromFile(FromFile* fromFile, FILE *outputFile){
    if (isFromFileInitiated(fromFile)) {
        FILE *openedFile = fopen(fromFile->filename, "r");
        fseek(openedFile, fromFile->startPos, SEEK_SET);
        long size = getFromFileSize(fromFile);
        long remainingSize = size;
        char buffer[1024];
        while (remainingSize > 0) {
            long toRead = remainingSize > 1024 ? 1024 : remainingSize;
            if (toRead != fread(buffer, 1, toRead*sizeof(char), openedFile)) {
                fclose(openedFile);
                return false;
            };
            if (toRead != fwrite(buffer, 1, toRead*sizeof(char), outputFile)) {
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

bool isFromFileInitiated(FromFile* fromFile){
    return fromFile->initiated;
}

void freeFromFile(FromFile* fromFile){
    free(fromFile->filename);
    free(fromFile);
}
