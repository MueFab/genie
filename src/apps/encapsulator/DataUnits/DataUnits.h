//
// Created by bscuser on 16/02/18.
//

#ifndef MPEGG_CAPSULATOR_DATAUNITS_H
#define MPEGG_CAPSULATOR_DATAUNITS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "../Boxes.h"
#include "../DataStructures/FromFile.h"

struct DataUnitAccessUnit_ {
    uint32_t accessUnitId;
    uint8_t numBlocks;
    uint8_t parameterSetId;
    ClassType AU_type;
    uint32_t readsCount;

    uint16_t mmThreshold;
    uint32_t mmCount;

    SequenceID ref_sequenced_ID;
    uint64_t ref_start_position;
    uint64_t ref_end_position;

    SequenceID sequenceId;
    uint64_t auStartPosition;
    uint64_t auEndPosition;
    uint64_t extendedAUStartPosition;
    uint64_t extendedAUEndPosition;

    // missing signatures;

    Vector* blocks;
    Vector* blocksHeaders;
};

typedef struct DataUnitAccessUnit_ DataUnitAccessUnit;

typedef struct {
    Vector* parameters;
    Vector* references;
    Vector* dataUnitsAccessUnits;
} DataUnits;

DataUnits* initDataUnits();
DataUnits* initDataUnitsTakeOverPointers(Vector* parameters, Vector* accessUnits);
void freeDataUnits(DataUnits* dataUnits);
int parseDataUnits(FILE* inputFile, DataUnits** dataUnits, char* filename);
int addDataUnitDatasetParameters(DataUnits* dataUnits, DatasetParameters* datasetParameters);
int addDataUnitParameters(DataUnits* dataUnits, DataUnitParametersSet* parametersSet);
int addDataUnitAccessUnitContainer(DataUnits* dataUnits, DatasetContainer* datasetContainer,
                                   AccessUnitContainer* accessUnitContainer);
int addDataUnitAccessUnit(DataUnits* dataUnits, DataUnitAccessUnit* dataUnitAccessUnit);
int getDataUnitsParameters(DataUnits* dataUnits, Vector** dataUnitsParameters);
int getDataUnitsAccessUnits(DataUnits* dataUnits, Vector** dataUnitsAccessUnits);
int getDataUnitsParametersById(DataUnits* dataUnits, uint16_t parameterSetId, DataUnitParametersSet** parametersSet);
bool writeDataUnits(DataUnits* dataUnits, FILE* outputStream);

DataUnitAccessUnit* getDataUnitAccessUnit(DatasetContainer* datasetContainer, SequenceID sequenceId,
                                          uint8_t class_index, uint32_t au_id);

DataUnitParametersSet* initParametersSet(uint16_t parent_parameter_setId, uint16_t parameter_setId,
                                         Encoding_ParametersType* encoding_parameters);
void freeDataUnitsParametersSet(DataUnitParametersSet* parameters);

typedef struct {
    uint16_t seq_count;
    uint16_t* sequence_ID;
    uint64_t* sequence_start;
    uint64_t* sequence_end;
    char** ref_sequence;
} DataUnitRawReference;

DataUnitRawReference* initDataUnitRawReference();
void addRawSequence(DataUnitRawReference* dataUnitRawReference, uint16_t sequenceId, uint64_t sequenceStart,
                    uint64_t sequenceEnd, char* ref_sequence);
void freeDataUnitRawReference(DataUnitRawReference* dataUnitRawReference);
DataUnitRawReference* parseRawReference(char* path);
int writeRawReference(DataUnitRawReference* dataUnitRawReferenceToWrite, char* path);
uint64_t getSizeDataUnitRawReference(DataUnitRawReference* dataUnitRawReference);

typedef struct {
    uint8_t descriptorId;
    uint32_t blockSize;
} DataUnitBlockHeader;

void freeDataUnitBlockHeader(DataUnitBlockHeader* dataUnitBlockHeader);
bool writeDataUnitBlockHeader(FILE* outputFile, DataUnitBlockHeader* dataUnitBlockHeader);
DataUnitBlockHeader* initDataUnitBlockHeader(uint8_t descriptorId, uint32_t blockSize);

DataUnitAccessUnit* initDataUnitAccessUnit(uint32_t accessUnitId, uint8_t numBlocks, uint16_t parameterSetId,
                                           ClassType AU_type, uint32_t readsCount, uint16_t mmThreshold,
                                           uint32_t mmCount, SequenceID referenceSequenceId, uint64_t referenceStartPos,
                                           uint64_t referenceEndPos, SequenceID sequenceId, uint64_t auStartPosition,
                                           uint64_t auEndPosition, uint64_t extendedAUStartPosition,
                                           uint64_t extendedAUEndPosition);
bool addBlockToDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit, Block* block,
                                  DataUnitBlockHeader* blockHeader);
int parseDataUnitAccessUnit(DataUnitAccessUnit** dataUnitAccessUnit, DataUnits* dataUnits, bool multipleAlignmentsFlag,
                            FILE* inputFile, char* filename, uint64_t size);
bool writeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit, bool multipleAlignmentsFlag, uint8_t datasetType,
                             bool pos40Bits, FILE* outputFile);
void freeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit);
int getDataUnitAccessUnitId(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* accessUnitId);
int getDataUnitNumBlocks(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t* numBlocks);
int getDataUnitParameterSetId(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t* parameterSetId);
int getDataUnitAUType(DataUnitAccessUnit* dataUnitAccessUnit, ClassType* auType);
int getDataUnitReadsCount(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* readsCount);
int getDataUnitMMThreshold(DataUnitAccessUnit* dataUnitAccessUnit, uint16_t* mmThreshold);
int getDataUnitMMCount(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* mmCount);
int getDataUnitSequenceId(DataUnitAccessUnit* dataUnitAccessUnit, SequenceID* sequenceId);
int getDataUnitAuStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* auStartPosition);
int getDataUnitAuEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* auEndPosition);
int getDataUnitReferenceSequenceId(DataUnitAccessUnit* dataUnitAccessUnit, SequenceID* refSequenceID);
int getDataUnitReferenceStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* referenceStart);
int getDataUnitReferenceEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* referenceEnd);
int getDataUnitExtendedAuStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* extendedAuStartPosition);
int getDataUnitExtendedAuEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* extendedAuEndPosition);
int getDataUnitBlock(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t block_i, Block** block);
bool hasParameter(DataUnits* dataUnits, uint8_t parametersId);

#endif  // MPEGG_CAPSULATOR_DATAUNITS_H
