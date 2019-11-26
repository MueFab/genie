#include "../utils.h"
#include "DataUnits.h"

DataUnitRawReference* initDataUnitRawReference() {
    DataUnitRawReference* dataUnitRawReference = (DataUnitRawReference*)malloc(sizeof(DataUnitRawReference));
    if (dataUnitRawReference == NULL) {
        return dataUnitRawReference;
    }
    dataUnitRawReference->seq_count = 0;
    dataUnitRawReference->sequence_ID = (uint16_t*)malloc(sizeof(uint16_t) * 65535);
    dataUnitRawReference->sequence_start = (uint64_t*)malloc(sizeof(uint64_t) * 65535);
    dataUnitRawReference->sequence_end = (uint64_t*)malloc(sizeof(uint64_t) * 65535);
    dataUnitRawReference->ref_sequence = calloc(65535, sizeof(char**));

    if (dataUnitRawReference->sequence_ID == NULL || dataUnitRawReference->sequence_start == NULL ||
        dataUnitRawReference->sequence_end == NULL || dataUnitRawReference->ref_sequence == NULL) {
        freeDataUnitRawReference(dataUnitRawReference);
        return NULL;
    }
    return dataUnitRawReference;
}

void freeDataUnitRawReference(DataUnitRawReference* dataUnitRawReference) {
    free(dataUnitRawReference->sequence_ID);
    free(dataUnitRawReference->sequence_start);
    free(dataUnitRawReference->sequence_end);
    for (uint16_t seq_i = 0; seq_i < dataUnitRawReference->seq_count; seq_i++) {
        free(dataUnitRawReference->ref_sequence[seq_i]);
    }
    free(dataUnitRawReference->ref_sequence);
    free(dataUnitRawReference);
}

void addRawSequence(DataUnitRawReference* dataUnitRawReference, uint16_t sequenceId, uint64_t sequenceStart,
                    uint64_t sequenceEnd, char* ref_sequence) {
    dataUnitRawReference->sequence_ID[dataUnitRawReference->seq_count] = sequenceId;
    dataUnitRawReference->sequence_start[dataUnitRawReference->seq_count] = sequenceStart;
    dataUnitRawReference->sequence_end[dataUnitRawReference->seq_count] = sequenceEnd;
    dataUnitRawReference->ref_sequence[dataUnitRawReference->seq_count] = ref_sequence;
    dataUnitRawReference->seq_count++;
}

DataUnitRawReference* parseRawReference(char* path) {
    DataUnitRawReference* dataUnitRawReference = initDataUnitRawReference();
    if (dataUnitRawReference == NULL) {
        fprintf(stderr, "Could not create Data unit raw reference.\n");
        freeDataUnitRawReference(dataUnitRawReference);
        return NULL;
    }
    FILE* inputFile = fopen(path, "rb");
    if (inputFile == NULL) {
        fprintf(stderr, "Could not open the file.\n");
        freeDataUnitRawReference(dataUnitRawReference);
        return NULL;
    }
    uint16_t sequenceCountBE;
    if (fread(&sequenceCountBE, sizeof(sequenceCountBE), 1, inputFile) != 1) {
        fprintf(stderr, "Could not read the number of sequences.\n");
        freeDataUnitRawReference(dataUnitRawReference);
        return NULL;
    };

    uint16_t sequenceCount = bigToNativeEndian16(sequenceCountBE);

    for (uint16_t sequence_i = 0; sequence_i < sequenceCount; sequence_i++) {
        uint16_t sequenceIdBE;
        if (1 != fread(&sequenceIdBE, sizeof(uint16_t), 1, inputFile)) {
            fprintf(stderr, "Could not read sequence id.\n");
            freeDataUnitRawReference(dataUnitRawReference);
            return NULL;
        }
        uint16_t sequenceId = bigToNativeEndian16(sequenceIdBE);
        dataUnitRawReference->sequence_ID[sequence_i] = sequenceId;

        uint64_t startPos = 0;
        if (1 != fread(((uint8_t*)&startPos) + 4, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&startPos) + 3, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&startPos) + 2, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&startPos) + 1, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&startPos), sizeof(uint8_t), 1, inputFile)) {
            fprintf(stderr, "Could not read start position.\n");
            freeDataUnitRawReference(dataUnitRawReference);
            return NULL;
        }
        dataUnitRawReference->sequence_start[sequence_i] = startPos;

        uint64_t endPos = 0;
        if (1 != fread(((uint8_t*)&endPos) + 4, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&endPos) + 3, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&endPos) + 2, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&endPos) + 1, sizeof(uint8_t), 1, inputFile) ||
            1 != fread(((uint8_t*)&endPos), sizeof(uint8_t), 1, inputFile)) {
            fprintf(stderr, "Could not read end position.\n");
            freeDataUnitRawReference(dataUnitRawReference);
            return NULL;
        }
        dataUnitRawReference->sequence_end[sequence_i] = endPos;

        uint64_t sequenceSize = endPos - startPos;
        char* sequence = malloc(sizeof(char) * sequenceSize);
        if (sequence == NULL) {
            fprintf(stderr, "Could not allocate sequence.\n");
            freeDataUnitRawReference(dataUnitRawReference);
            return NULL;
        }

        dataUnitRawReference->ref_sequence[sequence_i] = sequence;

        if (sequenceSize != fread(sequence, sizeof(char), sequenceSize, inputFile)) {
            fprintf(stderr, "Could not read sequence.\n");
            freeDataUnitRawReference(dataUnitRawReference);
            return NULL;
        }
    }
    return dataUnitRawReference;
}

int writeRawReference(DataUnitRawReference* dataUnitRawReferenceToWrite, char* path) {
    FILE* outputFile = fopen(path, "wb");
    if (outputFile == NULL) {
        fprintf(stderr, "Could not open the file.\n");
        return -1;
    }

    if (!utils_write(0, outputFile)) {
        fprintf(stderr, "Could not write the type of the data unit");
        return -1;
    }

    uint64_t size = getSizeDataUnitRawReference(dataUnitRawReferenceToWrite);
    if (!writeBigEndian64ToFile(size, outputFile)) {
        fprintf(stderr, "Could not write the size.\n");
        return -1;
    }

    uint16_t sequenceCountBE = nativeToBigEndian16(dataUnitRawReferenceToWrite->seq_count);
    if (fwrite(&sequenceCountBE, sizeof(sequenceCountBE), 1, outputFile) != 1) {
        fprintf(stderr, "Could not write the number of sequences.\n");
        return -1;
    };

    uint16_t sequenceCount = bigToNativeEndian16(sequenceCountBE);

    for (uint16_t sequence_i = 0; sequence_i < sequenceCount; sequence_i++) {
        uint16_t sequenceIdBE = nativeToBigEndian16(dataUnitRawReferenceToWrite->sequence_ID[sequence_i]);
        if (1 != fwrite(&sequenceIdBE, sizeof(sequenceIdBE), 1, outputFile)) {
            fprintf(stderr, "Could not write sequence id.\n");
            return -1;
        }

        uint64_t startPos = dataUnitRawReferenceToWrite->sequence_start[sequence_i];
        uint64_t startPosBE = nativeToBigEndian64(startPos);

        if (5 != fwrite(((uint8_t*)&startPosBE) + 3, sizeof(uint8_t), 5, outputFile)) {
            fprintf(stderr, "Could not write start position.\n");
            return -1;
        }

        uint64_t endPos = dataUnitRawReferenceToWrite->sequence_end[sequence_i];
        uint64_t endPosBE = nativeToBigEndian64(endPos);

        if (5 != fwrite(((uint8_t*)&endPosBE) + 3, sizeof(uint8_t), 5, outputFile)) {
            fprintf(stderr, "Could not write start position.\n");
            return -1;
        }

        uint64_t sequenceSize = endPos - startPos;
        if (sequenceSize !=
            fwrite(dataUnitRawReferenceToWrite->ref_sequence[sequence_i], sizeof(char), sequenceSize, outputFile)) {
            fprintf(stderr, "Could not write sequence.\n");
            return -1;
        }
    }
    fclose(outputFile);
    return 0;
}

uint64_t getSizeDataUnitRawReference(DataUnitRawReference* dataUnitRawReference) {
    uint64_t size = 0;
    size += 1;  // type
    size += 8;  // size
    size += 2;  // sequenceCount
    for (uint16_t sequence_i = 0; sequence_i < dataUnitRawReference->seq_count; sequence_i++) {
        size += 2;  // sequenceId
        size += 5;  // startPos
        size += 5;  // endPos
        size += dataUnitRawReference->sequence_end[sequence_i] - dataUnitRawReference->sequence_start[sequence_i];
    }
    return size;
}
