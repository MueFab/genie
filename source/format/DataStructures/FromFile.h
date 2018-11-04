//
// Created by gencom on 26/09/17.
//

#ifndef MPEGG_CAPSULATOR_FROMFILE_H
#define MPEGG_CAPSULATOR_FROMFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct{
    char* filename;
    long startPos;
    long endPos;
    bool initiated;
} FromFile;

FromFile* initFromFileWithFilename(const char* filename);
FromFile* initFromFile();
FromFile* initFromFileWithFilenameAndBoundaries(char *filename, long startPos, long endPos);
long getFromFileSize(FromFile* fromFile);
bool writeFromFile(FromFile* fromFile, FILE *outputFile);
bool isFromFileInitiated(FromFile* fromFile);
void freeFromFile(FromFile* fromFile);

#ifdef __cplusplus
}
#endif

#endif //MPEGG_CAPSULATOR_FROMFILE_H
