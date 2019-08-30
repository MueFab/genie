//
// Created by gencom on 26/09/17.
//

#ifndef MPEGG_CAPSULATOR_FROMFILE_H
#define MPEGG_CAPSULATOR_FROMFILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    char* filename;
    uint64_t startPos;
    uint64_t endPos;
    bool initiated;
} FromFile;

FromFile* initFromFileWithFilename(char* filename);
FromFile* initFromFile();
FromFile* initFromFileWithFilenameAndBoundaries(char* filename, uint64_t startPos, uint64_t endPos);
uint64_t getFromFileSize(FromFile* fromFile);
bool writeFromFile(FromFile* fromFile, FILE* outputFile);
bool isFromFileInitiated(FromFile* fromFile);
void freeFromFile(FromFile* fromFile);
FromFile* cloneFromFile(FromFile* originalFromFile);

#endif  // MPEGG_CAPSULATOR_FROMFILE_H
