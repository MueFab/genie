#ifndef MPEGG_REFERENCE_SW_FASTAREADER_H
#define MPEGG_REFERENCE_SW_FASTAREADER_H

#define FASTAREADER_FASTA_MAX_LINE_LENGTH 4096

#include <stdio.h>

typedef struct {
    size_t numberSequences;
    char** sequenceNames;
    char** sequence;
} FastaData;

FastaData* getFASTA(char* filePath);
void freeFastaData(FastaData* data);

#endif  // MPEGG_REFERENCE_SW_FASTAREADER_H
