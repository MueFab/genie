#include "FastaReader.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void addSequence(FastaData* fastaData, char* sequenceName, char* sequence);

FastaData* getFASTA(char* filePath) {
    FastaData* fastaData;
    fastaData = calloc(1, sizeof(FastaData));

    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open FASTA reference");
        free(fastaData);
        return NULL;
    }

    char readLineBuffer[FASTAREADER_FASTA_MAX_LINE_LENGTH];

    if ((fgets(readLineBuffer, FASTAREADER_FASTA_MAX_LINE_LENGTH, file)) == NULL) {
        freeFastaData(fastaData);
        return NULL;
    }
    while (!feof(file)) {
        if (readLineBuffer[0] != '>') {
            freeFastaData(fastaData);
            return NULL;
        }

        char* sequenceNameToken = strtok(readLineBuffer + 1, " \t\n");
        char* sequenceName = calloc((strlen(sequenceNameToken) + 1), sizeof(char));
        strcpy(sequenceName, sequenceNameToken);

        char* sequence = calloc(FASTAREADER_FASTA_MAX_LINE_LENGTH, sizeof(char));
        uint64_t currenLength = 0;
        uint64_t currentAllocate = FASTAREADER_FASTA_MAX_LINE_LENGTH;

        while (fgets(readLineBuffer, FASTAREADER_FASTA_MAX_LINE_LENGTH, file)) {
            if (readLineBuffer[0] == '>') {
                break;
            }
            size_t obtainedSnippetLength = strlen(readLineBuffer);
            for (size_t pos = 0; pos < obtainedSnippetLength + 1; pos++) {
                char toWrite;
                if (pos == obtainedSnippetLength) {
                    if (readLineBuffer[pos - 1] != '\n') {
                        toWrite = '\0';
                    } else {
                        break;
                    }
                } else {
                    toWrite = readLineBuffer[pos];
                }
                if (toWrite == '\n') {
                    continue;
                }
                sequence[currenLength] = toWrite;
                currenLength++;
                if (currenLength == currentAllocate) {
                    sequence = realloc(sequence, sizeof(char) * (currentAllocate + FASTAREADER_FASTA_MAX_LINE_LENGTH));
                    currentAllocate += FASTAREADER_FASTA_MAX_LINE_LENGTH;
                }
            }
        }
        sequence[currenLength] = 0;
        addSequence(fastaData, sequenceName, sequence);
    }

    return fastaData;
}

void freeFastaData(FastaData* data) {
    for (size_t sequence_i = 0; sequence_i < data->numberSequences; sequence_i++) {
        free(data->sequenceNames[sequence_i]);
        free(data->sequence[sequence_i]);
    }
    free(data->sequenceNames);
    free(data->sequence);
    free(data);
}

void addSequence(FastaData* fastaData, char* sequenceName, char* sequence) {
    fastaData->sequenceNames = realloc(fastaData->sequenceNames, sizeof(char*) * (fastaData->numberSequences + 1));
    fastaData->sequence = realloc(fastaData->sequence, sizeof(char*) * (fastaData->numberSequences + 1));

    fastaData->sequenceNames[fastaData->numberSequences] = sequenceName;
    fastaData->sequence[fastaData->numberSequences] = sequence;

    fastaData->numberSequences++;
}
