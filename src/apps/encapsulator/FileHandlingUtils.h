/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#ifndef GENIE_FILEHANDLINGUTILS_H
#define GENIE_FILEHANDLINGUTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <errno.h>

typedef struct{
    char refUri[1024];
    uint8_t numberSequences;
    char** sequenceName;
    char refName[1024];
} Ref_information;

Ref_information readRefInformation(char* refInfoFilePath);

int createMPEGGFileNoMITFromByteStream(const char *fileName, char* refInfoPath, char* outputFileName);

#endif //GENIE_FILEHANDLINGUTILS_H
