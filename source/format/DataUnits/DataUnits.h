//
// Created by bscuser on 16/02/18.
//

#ifndef MPEGG_CAPSULATOR_DATAUNITS_H
#define MPEGG_CAPSULATOR_DATAUNITS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <DataStructures/FromFile.h>
#include <Boxes.h>

struct DataUnitAccessUnit_{
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

    //missing signatures;

    Vector* blocks;
    Vector* blocksHeaders;
};

typedef struct DataUnitAccessUnit_ DataUnitAccessUnit;

typedef struct {
    uint8_t* datasetgroupid;
    uint16_t* datasetid;
    uint8_t* parameterSetId_DatasetScope;
    uint8_t* parameterSetId_DataUnitsScope;
    size_t allocatedElements;
    uint8_t createdElements;
} AllocatedDataUnitParametersIdsInfo;

typedef struct {
    AllocatedDataUnitParametersIdsInfo allocatedDataUnitParametersIdsInfo;
    Vector* parameters;
    Vector* references;
    Vector* dataUnitsAccessUnits;
} DataUnits;

DataUnits* initDataUnits();
void freeDataUnits(DataUnits* dataUnits);
int parseDataUnits(FILE* inputFile, DataUnits** dataUnits, char* filename);
int addDataUnitDatasetParameters(DataUnits *dataUnits, DatasetParameters *datasetParameters,
                                 uint8_t *parametersIdDataUnitsScope);
bool getParametersIdDataUnitsScope(
        DataUnits *dataUnits,
        uint8_t datasetgroupId,
        uint16_t datasetId,
        uint8_t parameterIdDatasetScope,
        uint8_t *parameterIdDataUnitsScope
);
int addDataUnitParameters(DataUnits* dataUnits, ParametersSet* parametersSet);
int addDataUnitAccessUnitContainer(
        DataUnits* dataUnits,
        DatasetContainer* datasetContainer,
        AccessUnitContainer* accessUnitContainer
);
int addDataUnitAccessUnit(DataUnits* dataUnits, DataUnitAccessUnit* dataUnitAccessUnit);
int getDataUnitsParameters(DataUnits* dataUnits, Vector** dataUnitsParameters);
int getDataUnitsAccessUnits(DataUnits* dataUnits, Vector** dataUnitsAccessUnits);
int getDataUnitsParametersById(DataUnits* dataUnits, uint16_t parameterSetId, ParametersSet** parametersSet);

DataUnitAccessUnit* getDataUnitAccessUnit(DatasetContainer *datasetContainer, SequenceID sequenceId,
                                          uint8_t class_index,
                                          uint32_t au_id);

ParametersSet * initParametersSet(
    uint16_t parent_parameter_setId,
    uint16_t parameter_setId,
    uint8_t dataset_type,
    uint8_t alphabet_ID,
    uint32_t reads_length,
    uint8_t number_of_template_segments_minus1,
    uint32_t max_au_data_unit_size,
    bool pos_40_bits,
    uint8_t qv_depth,
    uint8_t as_depth,
    ByteArray* data
);
void freeDataUnitsParametersSet(ParametersSet* parameters);

typedef struct {
    FromFile* fromFile;
} DataUnitRawReference;

typedef struct {
    FromFile* fromFile;
} DataUnitParameterSet;



typedef struct {
    uint8_t descriptorId;
    uint32_t blockSize;
} DataUnitBlockHeader;

void freeDataUnitBlockHeader(DataUnitBlockHeader *dataUnitBlockHeader);
bool writeDataUnitBlockHeader(FILE *outputFile, DataUnitBlockHeader *dataUnitBlockHeader);
DataUnitBlockHeader* initDataUnitBlockHeader(uint8_t descriptorId, uint32_t blockSize);

DataUnitAccessUnit* initDataUnitAccessUnit(
        uint32_t accessUnitId,
        uint8_t numBlocks,
        uint16_t parameterSetId,
        ClassType AU_type,
        uint32_t readsCount,
        uint16_t mmThreshold,
        uint32_t mmCount,
        SequenceID referenceSequenceId,
        uint64_t referenceStartPos,
        uint64_t referenceEndPos,
        SequenceID sequenceId,
        uint64_t auStartPosition,
        uint64_t auEndPosition,
        uint64_t extendedAUStartPosition,
        uint64_t extendedAUEndPosition
);
bool addBlockToDataUnitAccessUnit(DataUnitAccessUnit *dataUnitAccessUnit, Block *block,
                                  DataUnitBlockHeader *blockHeader);
int parseDataUnitAccessUnit(DataUnitAccessUnit **dataUnitAccessUnit, DataUnits* dataUnits, bool multipleAlignmentsFlag, FILE *inputFile,
                            char *filename, uint64_t size);
bool writeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit, bool multipleAlignmentsFlag, uint8_t datasetType, bool pos40Bits, FILE* outputFile);
void freeDataUnitAccessUnit(DataUnitAccessUnit* dataUnitAccessUnit);
int getDataUnitAccessUnitId(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* accessUnitId);
int getDataUnitNumBlocks(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t* numBlocks);
int getDataUnitParameterSetId(DataUnitAccessUnit* dataUnitAccessUnit, uint8_t* parameterSetId);
int getDataUnitAUType(DataUnitAccessUnit *dataUnitAccessUnit, ClassType *auType);
int getDataUnitReadsCount(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* readsCount);
int getDataUnitMMThreshold(DataUnitAccessUnit* dataUnitAccessUnit, uint16_t* mmThreshold);
int getDataUnitMMCount(DataUnitAccessUnit* dataUnitAccessUnit, uint32_t* mmCount);
int getDataUnitSequenceId(DataUnitAccessUnit *dataUnitAccessUnit, SequenceID *sequenceId);
int getDataUnitAuStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* auStartPosition);
int getDataUnitAuEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* auEndPosition);
int getDataUnitReferenceSequenceId(DataUnitAccessUnit* dataUnitAccessUnit, SequenceID *refSequenceID);
int getDataUnitReferenceStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* referenceStart);
int getDataUnitReferenceEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* referenceEnd);
int getDataUnitExtendedAuStartPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* extendedAuStartPosition);
int getDataUnitExtendedAuEndPosition(DataUnitAccessUnit* dataUnitAccessUnit, uint64_t* extendedAuEndPosition);
int getDataUnitBlock(DataUnitAccessUnit *dataUnitAccessUnit, uint8_t block_i, Block** block);

#endif //MPEGG_CAPSULATOR_DATAUNITS_H
