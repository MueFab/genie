/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#ifndef GENIE_FILEHANDLINGUTILS_H
#define GENIE_FILEHANDLINGUTILS_H

#include <memory.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <mpegg_p2/data_unit.h>
#include <util/bitreader.h>
#include <fstream>

// typedef struct {
//    char refUri[1024];
//    uint8_t numberSequences;
//    char** sequenceName;
//    char refName[1024];
//} Ref_information;
//
// Ref_information readRefInformation(char* refInfoFilePath);

int createMPEGGFileNoMITFromByteStream(const char* fileName, const char* outputFileName);

#endif  // GENIE_FILEHANDLINGUTILS_H
