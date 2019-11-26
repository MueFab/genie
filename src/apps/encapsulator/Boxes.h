//
// Created by gencom on 24/07/17.
//

#ifndef MPEGG_CAPSULATOR_BOXES_H
#define MPEGG_CAPSULATOR_BOXES_H

#include <stdint.h>
#include <stdio.h>
#include "ClassesSet.h"
#include "DataStructures/EncodingParameters.h"
#include "DataStructures/FromFile.h"
#include "DataStructures/Signatures/Signatures.h"
#include "treeUint64.h"
#include "vector.h"
#include "vectorUint64.h"

#define MAJOR_BRAND_SIZE 6
#define MINOR_VERSION_SIZE 4
#define BRAND_SIZE 4
#define BOX_SIZE 8
#define BOX_HEADER_SIZE (BRAND_SIZE + BOX_SIZE)
typedef uint8_t DatasetGroupId;
typedef uint16_t DatasetId;
typedef uint8_t ReferenceId;
typedef uint16_t ReferenceMajorVersion;
typedef uint16_t ReferenceMinorVersion;
typedef uint16_t ReferencePatchVersion;

extern const char fileHeaderBoxName[];
extern const char datasetsGroupBoxName[];
extern const char datasetsGroupHeaderName[];
extern const char datasetsGroupMetadataName[];
extern const char datasetsGroupProtectionName[];
extern const char datasetsGroupReferencesName[];
extern const char datasetsGroupReferenceMetadataName[];
extern const char datasetsGroupLabelName[];
extern const char datasetsGroupLabelsListName[];

extern const char datasetContainerName[];
extern const char datasetMasterIndexTableName[];
extern const char datasetLabelsListName[];
extern const char datasetHeaderName[];
extern const char datasetMetadataName[];
extern const char datasetParametersName[];
extern const char datasetProtectionName[];

extern const char streamContainerName[];
extern const char streamHeaderName[];
extern const char streamMetadataName[];
extern const char streamProtectionName[];

extern const char accessUnitContainerName[];
extern const char accessUnitHeaderName[];
extern const char accessUnitInformationName[];
extern const char accessUnitProtectionName[];

typedef unsigned char Byte;

typedef uint8_t DatasetType;
typedef uint32_t AUs_count;
typedef uint8_t Classes_count;
typedef uint16_t Descriptors_count;

typedef struct {
    uint64_t allocated_size;
    uint64_t size;
    Byte* byte_array;
} ByteArray;

typedef enum {
    CLASS_TYPE_CLASS_P = 1,
    CLASS_TYPE_CLASS_N = 2,
    CLASS_TYPE_CLASS_M = 3,
    CLASS_TYPE_CLASS_I = 4,
    CLASS_TYPE_CLASS_HM = 5,
    CLASS_TYPE_CLASS_U = 6
} ClassTypeEnum;

typedef struct {
    ClassTypeEnum classType;
} ClassType;

typedef struct DatasetsGroupHeader_ DatasetsGroupHeader;
typedef struct DatasetsGroupMetadata_ DatasetsGroupMetadata;
typedef struct DatasetsGroupProtection_ DatasetsGroupProtection;
typedef struct DatasetsGroupReferenceGenome_ DatasetsGroupReferenceGenome;
typedef struct DatasetsGroupReferenceMetadata_ DatasetsGroupReferenceMetadata;
typedef struct DatasetContainer_ DatasetContainer;
typedef struct DatasetsGroupLabelsList_ DatasetsGroupLabelsList;
typedef struct Label_ Label;

typedef struct {
    DatasetsGroupHeader* datasetsGroupHeader;
    Vector* datasetsGroupReferenceGenomes;
    Vector* datasetsGroupReferenceMetadatas;
    DatasetsGroupMetadata* datasetsGroupMetadata;
    DatasetsGroupProtection* datasetsGroupProtection;
    DatasetsGroupLabelsList* datasetsGroupLabelsList;
    Vector* datasetsContainer;
    size_t seekPosition;
} DatasetsGroupContainer;

typedef enum {
    REFERENCE_TYPE_MPEGG_REF = 0,
    REFERENCE_TYPE_RAW_REF = 1,
    REFERENCE_TYPE_FASTA_REF = 2
} ReferenceTypeEnum;

struct DatasetsGroupHeader_ {
    DatasetGroupId datasetGroupId;
    uint8_t versionNumber;
    VectorUint64* datasetsId;
    size_t seekPosition;
    bool hasSeek;
};

struct DatasetsGroupMetadata_ {
    ByteArray* metadata;
    size_t seekPosition;
    bool hasSeek;
};

struct DatasetsGroupReferenceMetadata_ {
    ByteArray* metadata;
    long seekPosition;
};

struct DatasetsGroupProtection_ {
    ByteArray* protection;
    size_t seekPosition;
    bool hasSeek;
};

typedef struct {
    char* ref_uri;
    uint8_t reference_type;
    DatasetGroupId externalDatasetGroupId;
    DatasetId externalDatasetId;
    Vector* checksums;
    uint8_t checksumAlgorithm;
} ExternalReference;

typedef struct {
    ReferenceId referenceId;
    size_t referenceNameBufferLength;
    char* referenceName;
    ReferenceMajorVersion referenceMajorVersion;
    ReferenceMinorVersion referenceMinorVersion;
    ReferencePatchVersion referencePatchVersion;
} ReferenceInformation;

struct DatasetsGroupLabelsList_ {
    DatasetGroupId datasetsGroupId;
    Vector* labels;
    long seekPosition;
};
DatasetsGroupLabelsList* initDatasetsGroupLabelsList(DatasetGroupId datasetGroupId);
void freeDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList);
void addLabel(DatasetsGroupLabelsList* datasetsGroupLabelsList, Label* label);
uint64_t getSizeContentDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList);
uint64_t getSizeDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList);
bool writeContentDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList, FILE* outputFile);
bool writeDatasetsGroupLabelsList(DatasetsGroupLabelsList* datasetsGroupLabelsList, FILE* outputFile);
DatasetsGroupLabelsList* parseDatasetsGroupLabelsList(FILE* inputFile);
long getDatasetsGroupLabelsListSeekPosition(DatasetsGroupLabelsList* datasetsGroupLabelsList);

ReferenceInformation initReferenceInformation();
bool readReferenceInformation(ReferenceInformation* referenceInformation, FILE* inputFile);
size_t getDatasetGroupReferenceGenomeSeekPosition(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome);
bool resizeSequences(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, size_t newSize);
bool setSequenceName(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, size_t sequence_i, char* name);
bool setSequenceNamesVector(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, Vector* namesToCopy);

struct DatasetsGroupReferenceGenome_ {
    uint8_t dataset_group_id;
    bool isExternal;
    ExternalReference externalReference;
    DatasetGroupId internalRefDatasetGroupId;
    DatasetId internalRefDatasetId;
    ReferenceId referenceId;
    char* referenceName;
    ReferenceMajorVersion referenceMajorVersion;
    ReferenceMinorVersion referenceMinorVersion;
    ReferencePatchVersion referencePatchVersion;
    size_t seekPosition;
    Vector* sequences;
    bool hasSeek;
};

DatasetsGroupProtection* initDatasetsGroupProtection();
void freeDatasetsGroupProtection(DatasetsGroupProtection* datasetsGroupProtection);
bool defineDatasetsGroupProtectionContent(DatasetsGroupProtection* datasetsGroupProtection, char* filename);
bool writeDatasetsGroupProtectionContent(DatasetsGroupProtection* datasetsGroupProtection, FILE* outputFile);
bool writeDatasetsGroupProtection(DatasetsGroupProtection* datasetsGroupProtection, FILE* outputFile);
DatasetsGroupProtection* parseDatasetsGroupProtection(uint64_t boxContentSize, FILE* inputFile);
bool copyContentDatasetsGroupProtection(DatasetsGroupProtection* datasetsGroupProtection, char* content,
                                        uint64_t contentSize);
uint64_t getDatasetsGroupProtectionContentSize(DatasetsGroupProtection* datasetsGroupProtection);
uint64_t getDatasetsGroupProtectionSize(DatasetsGroupProtection* datasetsGroupProtection);
size_t getDatasetGroupProtectionSeekPosition(DatasetsGroupProtection* datasetsGroupProtection);

DatasetsGroupReferenceGenome* initDatasetsGroupReferenceGenomeExternal(
    uint8_t dataset_group_id, char* ref_uri, uint8_t referenceType, DatasetGroupId externalDatasetGroupId,
    DatasetId externalDatasetId, uint8_t checksumAlg, Vector* checksums, ReferenceId referenceId, char* referenceName,
    ReferenceMajorVersion majorVersion, ReferenceMinorVersion minorVersion, ReferencePatchVersion referencePathVersion);

DatasetsGroupReferenceGenome* initDatasetsGroupReferenceGenome(uint8_t dataset_group_id,
                                                               DatasetGroupId internalDatasetGroupId,
                                                               DatasetId internalDatasetId, ReferenceId referenceId,
                                                               char* referenceName, ReferenceMajorVersion majorVersion,
                                                               ReferenceMinorVersion minorVersion,
                                                               ReferencePatchVersion referencePathVersion);
void freeDatasetsGroupReferenceGenome(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome);
bool writeDatasetsGroupReferenceGenome(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome, FILE* outputFile);
uint64_t getDatasetsGroupReferenceGenomeContentSize(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome);
uint64_t getDatasetsGroupReferenceGenomeSize(DatasetsGroupReferenceGenome* datasetsGroupReferenceGenome);
DatasetsGroupReferenceGenome* parseDatasetsGroupReferenceGenome(FILE* inputFile);

typedef struct {
    char majorBrand[MAJOR_BRAND_SIZE];
    char minorVersion[4];
    Vector* compatibleBrands;
} FileHeader;

typedef struct {
    FileHeader* fileHeader;
    Vector* datasetsGroups;
} MPEGGFile;

typedef struct DatasetHeader_ DatasetHeader;
typedef struct StreamContainer_ StreamContainer;
typedef struct AccessUnitContainer_ AccessUnitContainer;
typedef struct DatasetMasterIndexTable_ DatasetMasterIndexTable;
typedef struct DatasetParameters_ DatasetParameters;
typedef struct DatasetProtection_ DatasetProtection;
typedef struct DatasetMetadata_ DatasetMetadata;
typedef struct AccessUnitHeader_ AccessUnitHeader;

struct DatasetContainer_ {
    DatasetHeader* datasetHeader;
    DatasetMasterIndexTable* datasetMasterIndexTable;
    Vector* datasetParameters;
    DatasetMetadata* datasetMetadata;
    DatasetProtection* datasetProtection;
    size_t seekPosition;
    Vector* streamContainers;
    Vector* accessUnitContainers;
    uint64_t*** accessUnitsOffsets;
    AccessUnitContainer**** accessUnitsContainers;
    bool accessUnitsOffsetsInitiatedWithValues;
    bool hasSeekPosition;
};

MPEGGFile* initFile();
void freeFile(MPEGGFile* file);
void setFileHeaderToFile(MPEGGFile* file, FileHeader* fileHeader);
void addDatasetsGroupToFile(MPEGGFile* file, DatasetsGroupContainer* datasetsGroupContainer);
bool writeFile(MPEGGFile* file, FILE* outputFile);
bool isFileValid(MPEGGFile* file);
MPEGGFile* parseFile(FILE* inputFile, char* filename);

bool generateFileSeekPoints(MPEGGFile* file);

FileHeader* getFileHeader(MPEGGFile* file);
DatasetsGroupContainer* getDatasetGroupContainerByIndex(MPEGGFile* file, size_t index);
DatasetsGroupContainer* getDatasetGroupContainerById(MPEGGFile* file, DatasetGroupId datasetGroupId);
unsigned long getDatasetsGroupContainersSize(MPEGGFile* file);

ByteArray* initByteArray();
ByteArray* initByteArrayCopying(ByteArray* byteArray);
ByteArray* initByteArrayWithSize(uint64_t size);
ByteArray* initByteArrayPassData(uint64_t size, Byte* data);
void freeByteArray(ByteArray* byteArray);
bool writeByteArray(ByteArray* byteArray, FILE* outputFile);
uint64_t getSizeByteArray(ByteArray* byteArray);
bool copyBytesSource(ByteArray* byteArray, Byte* source, uint64_t source_size);
ByteArray* cloneByteArray(ByteArray* sourceByteArray);

FileHeader* initFileHeader(char* minorVersion);
void freeFileHeader(FileHeader* fileHeader);
void addCompatibleBrandToFileHeader(FileHeader* fileHeader, char* brand);
bool writeFileHeader(FileHeader* fileHeader, FILE* outputFile);
bool writeFileHeaderContent(FileHeader* fileHeader, FILE* outputFile);
FileHeader* parseFileHeader(uint64_t boxContentSize, FILE* inputFile);
uint64_t getSizeFileHeader(FileHeader* fileHeader);

DatasetsGroupHeader* initDatasetsGroupHeader(DatasetGroupId datasetGroupId, uint8_t versionNumber);
void freeDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader);
void addDatasetId(DatasetsGroupHeader* datasetsGroupHeader, DatasetId datasetId);
bool writeDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader, FILE* outputFile);
bool writeDatasetsGroupHeaderContent(DatasetsGroupHeader* datasetsGroupHeader, FILE* outputFile);
DatasetsGroupHeader* parseDatasetsGroupHeader(uint64_t boxContentSize, FILE* inputFile);
uint64_t getSizeContentDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader);
uint64_t getSizeDatasetsGroupHeader(DatasetsGroupHeader* datasetsGroupHeader);
size_t getDatasetGroupHeaderSeekPosition(DatasetsGroupHeader* datasetsGroupHeader);

DatasetsGroupMetadata* initDatasetsGroupMetadata();
void freeDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata);
bool copyContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, char* content,
                                      uint64_t contentSize);
bool defineContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, char* filename);
bool writeDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, FILE* outputFile);
bool writeContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata, FILE* outputFile);
DatasetsGroupMetadata* parseDatasetsGroupMetadata(uint64_t boxContentSize, FILE* inputFile);
uint64_t getSizeDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata);
uint64_t getSizeContentDatasetsGroupMetadata(DatasetsGroupMetadata* datasetsGroupMetadata);
size_t getDatasetGroupMetadataSeekPosition(DatasetsGroupMetadata* datasetsGroupMetadata);

DatasetsGroupContainer* initDatasetsGroupContainer();
uint64_t getSizeContentDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer);
uint64_t getSizeDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer);
bool writeDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer, FILE* outputFile);
bool writeContentDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer, FILE* outputFile);
void setDatasetsGroupHeader(DatasetsGroupContainer* datasetsGroupContainer, DatasetsGroupHeader* datasetsGroupHeader);
void setDatasetsGroupReferenceGenomes(DatasetsGroupContainer* datasetsGroupContainer,
                                      Vector* datasetsGroupReferenceGenome);
void setDatasetsGroupReferenceMetadata(DatasetsGroupContainer* datasetsGroupContainer,
                                       Vector* datasetsGroupReferenceMetadatas);
void setDatasetsGroupMetadata(DatasetsGroupContainer* datasetsGroupContainer,
                              DatasetsGroupMetadata* datasetsGroupMetadata);
void setDatasetsGroupProtection(DatasetsGroupContainer* datasetsGroupContainer,
                                DatasetsGroupProtection* datasetsGroupProtection);
void setDatasetsGroupLabelsList(DatasetsGroupContainer* datasetsGroupContainer,
                                DatasetsGroupLabelsList* datasetsGroupLabelsList);
DatasetsGroupLabelsList* getDatasetsGroupLabelsList(DatasetsGroupContainer* datasetsGroupContainer);
void addDatasetsContainer(DatasetsGroupContainer* datasetsGroupContainer, DatasetContainer* datasetContainer);
void freeDatasetsGroupContainer(DatasetsGroupContainer* datasetsGroupContainer);
bool isDatasetsGroupContainerValid(DatasetsGroupContainer* datasetsGroupContainer);
DatasetsGroupContainer* parseDatasetsGroupContainer(FILE* inputFile, uint64_t boxContentSize, char* filename);
size_t getNumberDatasets(DatasetsGroupContainer* datasetsGroupContainer);
DatasetContainer* getDatasetContainerByIndex(DatasetsGroupContainer* datasetsGroupContainer, size_t index);
DatasetContainer* getDatasetContainerById(DatasetsGroupContainer* datasetsGroupContainer, DatasetId datasetId);
size_t getDatasetGroupContainerSeekPosition(DatasetsGroupContainer* datasetsGroupContainer);
bool generateDatasetsGroupSeekPoints(DatasetsGroupContainer* datasetsGroupContainer);
DatasetsGroupReferenceGenome* getReferenceGenome(DatasetsGroupContainer* datasetsGroupContainer,
                                                 ReferenceId referenceId);

DatasetsGroupReferenceMetadata* initDatasetsGroupReferenceMetadata();
void freeDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata);
bool copyContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                               char* content, uint64_t contentSize);
bool defineContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                                 char* filename);
bool writeDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                         FILE* outputFile);
bool writeContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata,
                                                FILE* outputFile);
DatasetsGroupReferenceMetadata* parseDatasetsGroupReferenceMetadata(uint64_t boxContentSize, FILE* inputFile);
uint64_t getSizeDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata);
uint64_t getSizeContentDatasetsGroupReferenceMetadata(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata);
long getDatasetGroupReferenceMetadataSeekPosition(DatasetsGroupReferenceMetadata* datasetsGroupReferenceMetadata);

// typedef struct DataUnitAccessUnit_ DataUnitAccessUnit;
DatasetContainer* initDatasetContainer();
void freeDatasetContainer(DatasetContainer* datasetContainer);
void addAccessUnitToDataset(DatasetContainer* datasetContainer, AccessUnitContainer* accessUnitContainer);
void addStreamContainerToDataset(DatasetContainer* datasetContainer, StreamContainer* streamContainer);
size_t getNumberStreamContainer(DatasetContainer* datasetContainer);
uint64_t getSizeContentDatasetContainer(DatasetContainer* datasetContainer);
uint64_t getSizeDatasetContainer(DatasetContainer* datasetContainer);
bool writeDatasetContainer(DatasetContainer* datasetContainer, FILE* outputFile);
bool writeContentDatasetContainer(DatasetContainer* datasetContainer, FILE* outputFile);
bool isDatasetContainerValid(DatasetContainer* datasetContainer);
StreamContainer* getStreamContainer(DatasetContainer* datasetContainer, size_t index);
void setDatasetHeader(DatasetContainer* datasetContainer, DatasetHeader* datasetHeader);
DatasetHeader* getDatasetHeader(DatasetContainer* datasetContainer);
void setDatasetMasterIndexTable(DatasetContainer* datasetContainer, DatasetMasterIndexTable* datasetMasterIndexTable);
void setDatasetParameters(DatasetContainer* datasetContainer, Vector* datasetParameters);
void setDatasetMetadata(DatasetContainer* datasetContainer, DatasetMetadata* datasetMetadata);
void setDatasetProtection(DatasetContainer* datasetContainer, DatasetProtection* datasetProtection);
DatasetContainer* parseDatasetContainer(uint64_t boxContentSize, FILE* inputFile, char* fileName);
size_t getDatasetContainerSeekPosition(DatasetContainer* datasetContainer);
bool generateDatasetSeekPoints(DatasetContainer* datasetContainer);
Vector* getDataUnitAccessUnits(DatasetContainer* datasetContainer);
int getNumberParameters(DatasetContainer* datasetContainer, size_t* value);
int getDatasetParameters(DatasetContainer* datasetContainer, size_t index, DatasetParameters** datasetParameters);
int getDatasetParametersById(DatasetContainer* datasetContainer, uint16_t id, DatasetParameters** datasetParameters);
size_t getMaxDatasetParametersId(DatasetContainer* datasetContainer);

struct DatasetMetadata_ {
    ByteArray* metadata;
    size_t seekPosition;
    bool hasSeek;
};

DatasetMetadata* initDatasetMetadata();
void freeDatasetMetadata(DatasetMetadata* datasetMetadata);
bool defineContentDatasetMetadata(DatasetMetadata* datasetMetadata, char* filename);
bool writeDatasetMetadata(DatasetMetadata* datasetMetadata, FILE* outputFile);
bool writeContentDatasetMetadata(DatasetMetadata* datasetMetadata, FILE* outputFile);
DatasetMetadata* parseDatasetMetadata(uint64_t boxContentSize, FILE* inputFile);
uint64_t getSizeContentDatasetMetadata(DatasetMetadata* datasetMetadata);
uint64_t getSizeDatasetMetadata(DatasetMetadata* datasetMetadata);
bool copyContentDatasetMetadata(DatasetMetadata* datasetMetadata, char* content, uint64_t contentSize);
size_t getDatasetMetadataSeekPosition(DatasetMetadata* datasetMetadata);

struct DatasetProtection_ {
    ByteArray* protection;
    bool hasSeek;
    size_t seekPosition;
};

DatasetProtection* initDatasetProtection();
void freeDatasetProtection(DatasetProtection* datasetProtection);
bool defineContentDatasetProtection(DatasetProtection* datasetProtection, char* filename);
bool writeContentDatasetProtection(DatasetProtection* datasetProtection, FILE* outputFile);
bool writeDatasetProtection(DatasetProtection* datasetProtection, FILE* outputFile);
uint64_t getSizeContentDatasetProtection(DatasetProtection* datasetProtection);
uint64_t getSizeDatasetProtection(DatasetProtection* datasetProtection);
DatasetProtection* parseDatasetProtection(uint64_t boxContentSize, FILE* inputFile);
bool copyContentDatasetProtection(DatasetProtection* datasetProtection, char* content, uint64_t contentSize);
size_t getDatasetProtectionSeekPosition(DatasetProtection* datasetProtection);

typedef struct {
    uint64_t* block_byte_offset;
    Descriptors_count descriptors_number;
} Block_offsets;
typedef struct {
    Tree** offsetsPerStream;
    uint64_t* sizeTrees;
    uint32_t numberStreams;
} OffsetsPerStream;

typedef struct {
    OffsetsPerStream* offsetsPerClass;
    int numberClasses;
} OffsetsPerClass;
typedef struct {
    uint64_t AU_byte_offset;
    uint64_t au_start_position;
    uint64_t au_end_position;
    uint16_t ref_sequence_id;
    uint64_t ref_start_position;
    uint64_t ref_end_position;
    uint64_t extended_au_start_position;
    uint64_t extended_au_end_position;
    Block_offsets block_byte_offset;
} AU_indexing;
typedef struct {
    AU_indexing* au_indexings;
    AUs_count number_AUs;
} AUs_index;
typedef struct {
    AUs_index* classIndex;
    uint16_t number_of_classes;
} Sequence_indexation;
typedef struct {
    Sequence_indexation* sequence_indexes;
    uint16_t number_sequences;
} Sequences_indexations;
typedef struct {
    uint16_t sequenceID;
} SequenceID;

bool getSequenceIdFromSequenceName(DatasetsGroupContainer* datasetsGroupContainer, DatasetContainer* datasetContainer,
                                   char* sequenceName, SequenceID* sequenceId);

typedef struct {
    uint32_t numberUAccessUnits;
    uint64_t* AU_byte_offset;
    uint16_t* ref_sequence_id;
    uint64_t* ref_start_position;
    uint64_t* ref_end_position;
    Signatures** signatures;
    uint32_t* uAccessUnitSizes;
    uint64_t** byteOffset;
    Tree** offsetsDescriptorUnmappedTree;
} UAccessUnitsIndex;

struct DatasetMasterIndexTable_ {
    Sequences_indexations sequence_indexes;
    UAccessUnitsIndex uAccessUnitsIndex;
    DatasetContainer* datasetContainer;
    size_t seekPosition;
    OffsetsPerClass* offsetsPerClass;
    bool hasSeek;
};

DatasetMasterIndexTable* initDatasetMasterIndexTable(DatasetContainer* datasetContainer);
void freeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable);
bool writeContentDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* outputFile);
bool writeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable, FILE* outputFile);
uint64_t getSizeContentDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable);
uint64_t getSizeDatasetMasterIndexTable(DatasetMasterIndexTable* datasetMasterIndexTable);
void setAUOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, ClassType classType,
                 uint64_t AU_id, uint64_t offset);
void setStartAndEnd(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, ClassType classType,
                    uint64_t AU_id, uint64_t start, uint64_t end);
void setExtendedStartAndEnd(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                            uint32_t AU_id, uint32_t extended_start, uint32_t extended_end);
void setOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, ClassType classType,
               uint32_t AU_i, uint16_t descriptor_i, uint64_t offset);
void insertFinalOffset(DatasetMasterIndexTable* datasetMasterIndexTable, ClassType classType, uint16_t classIndex,
                       uint16_t descriptorIndex, uint64_t offset);
void setUnalignedOffset(DatasetMasterIndexTable* datasetMasterIndexTable, uint32_t uAU_id, uint32_t uDescriptorId,
                        uint64_t offset);
Signatures* getSignatures(DatasetMasterIndexTable* datasetMasterIndexTable, int uAccessUnit_i);
DatasetMasterIndexTable* parseDatasetMasterIndexTable(DatasetContainer* datasetContainer, FILE* inputFile);
size_t getDatasetMasterIndexTableSeekPosition(DatasetMasterIndexTable* datasetMasterIndexTable);

bool getAccessUnitByteOffset(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceId, uint16_t classId,
                             uint32_t auId, uint64_t* byteOffset);
bool getBlockByteOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, ClassTypeEnum classId,
                        uint32_t auId, uint32_t descriptorId, uint64_t* byteOffset);
bool getNextBlockByteOffset(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, uint16_t classId,
                            uint32_t auId, uint32_t descriptorId, uint64_t* nextByteOffset);
uint64_t getAUStartPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, uint16_t classId,
                            uint32_t AU_id);
uint64_t getAUEndPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId, uint16_t classId,
                          uint32_t AU_id);
uint16_t getRefSequenceId(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceIndex, uint16_t classIndex,
                          uint32_t AU_id);
uint64_t getRefStartPosition(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceIndex,
                             uint16_t classIndex, uint32_t AU_id);
uint64_t getRefEndPosition(DatasetMasterIndexTable* datasetMasterIndexTable, uint16_t sequenceIndex, uint16_t classId,
                           uint32_t AU_id);
uint64_t getAUExtendedStartPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId,
                                    uint16_t classId, uint32_t AU_id);
uint64_t getAUExtendedEndPosition(DatasetMasterIndexTable* datasetMasterIndexTable, SequenceID sequenceId,
                                  uint16_t classId, uint32_t AU_id);

struct DatasetParameters_ {
    uint8_t dataset_group_ID;
    uint16_t dataset_ID;
    uint8_t parent_parameter_set_ID;
    uint8_t parameter_set_ID;
    Encoding_ParametersType* encoding_parameters;
    size_t seekPosition;
    bool hasSeek;
};

DatasetParameters* initDatasetParameters(uint8_t dataset_group_ID, uint16_t dataset_ID,
                                         uint16_t parent_parameter_set_ID, uint16_t parameter_set_ID);
DatasetParameters* initDatasetParametersWithParameters(uint8_t dataset_group_ID, uint16_t dataset_ID,
                                                       uint16_t parent_parameter_set_ID, uint16_t parameter_set_ID,
                                                       Encoding_ParametersType* encoding_parameters);
void freeDatasetParameters(DatasetParameters* datasetParameters);
bool defineContentDatasetParameters(DatasetParameters* datasetParameters, char* filename);
bool writeDatasetParameters(DatasetParameters* datasetParameters, FILE* outputFile);
bool writeContentDatasetParameters(DatasetParameters* datasetParameters, FILE* outputFile);
uint64_t getSizeContentDatasetParameters(DatasetParameters* datasetParameters);
uint64_t getSizeDatasetParameters(DatasetParameters* datasetParameters);
DatasetParameters* parseDatasetParameters(uint64_t boxContentSize, FILE* inputFile);
bool copyContentDatasetParameters(DatasetParameters* datasetParameters, char* content, uint64_t contentSize);
size_t getDatasetParametersSeekPosition(DatasetParameters* datasetParameters);

typedef struct {
    DatasetId datasetId;
    Vector* regions;
} RegionsForDataset;

typedef struct {
    uint16_t sequence_Id;
    ClassesSet classes;
    uint64_t start_pos;
    uint64_t end_pos;
} Region;

RegionsForDataset* initRegionsForDataset(DatasetId datasetId);
void freeRegionsForDataset(RegionsForDataset* regionsForDataset);
bool writeRegionsForDataset(RegionsForDataset* regionsForDataset, OutputBitstream* outputFile);
RegionsForDataset* parseRegionsForDataset(InputBitstream* inputFile);
uint64_t getRegionsForDatasetSize(RegionsForDataset* regionsForDataset);
void addRegion(RegionsForDataset* regionsForDataset, Region* region);

Region* initRegion(uint16_t sequence_Id, uint64_t start_pos, uint64_t end_pos);
void freeRegion(Region* region);
void addClass(Region* region, uint8_t classId);
unsigned long getNumberClasses(Region* region);
bool writeRegion(Region* region, OutputBitstream* outputFile);
uint64_t getContentSizeRegion(Region* region);
Region* parseRegion(InputBitstream* inputFile);

struct Label_ {
    char* labelId;
    Vector* regionsForDataset;
};

Label* initLabel(char* labelId);
void freeLabel(Label* label);
void addRegionsForDataset(Label* label, RegionsForDataset* regionsForDataset);
unsigned long getNumberRegionsForDataset(Label* label);
uint64_t getContentSizeLabel(Label* label);
uint64_t getSizeLabel(Label* label);
bool writeLabel(Label* label, FILE* outputFile);
bool writeContentLabel(Label* label, FILE* outputFile);
Label* parseLabel(FILE* inputFile);

struct DatasetHeader_ {
    DatasetGroupId datasetGroupId;
    DatasetId datasetId;
    char version[4];
    bool multipleAlignmentFlag;
    bool byteOffsetSizeFlag;
    bool posOffsetIsUint40;
    bool nonOverlappingAURange_flag;
    bool blockHeaderFlag;
    // if blockHeaderFlag
    bool mitFlag;
    bool classContiguosModeFlag;
    // else
    bool orderedBlocksFlag;
    uint16_t sequencesCount;
    ReferenceId referenceId;
    SequenceID* seqIds;
    uint32_t* seqBlocks;
    DatasetType datasetType;
    Classes_count numClasses;
    ClassType* classId;
    uint8_t* numDescriptors;
    uint8_t** descriptorId;
    uint8_t alphabetId;
    uint32_t numUClusters;
    bool uSignatureConstantLength;
    uint8_t uSignatureSize;
    uint8_t uSignatureLength;
    uint32_t numberUAccessUnits;
    uint32_t multipleSignatureBase;
    uint32_t threshold_0;  // use to store threshold 0 when seq_count = 0
    uint32_t* thresholds;
    size_t seekPosition;
    bool hasSeek;
};

DatasetHeader* initDatasetHeaderNoMIT(DatasetGroupId datasetGroupId, DatasetId datasetId, char* version,
                                      bool multipleAlignmentFlag, bool byteOffsetSizeFlag, bool posOffsetIsUint40,
                                      bool nonOverlappingAURange, bool blockHeaderFlag, uint16_t sequencesCount,
                                      ReferenceId referenceId, uint8_t datasetType, uint8_t numClasses,
                                      uint8_t alphabetId, uint32_t numUClusters, uint8_t uSignatureSize,
                                      uint8_t uSignatureLength, bool uSignatureConstantLength,
                                      uint32_t numberUAccessUnits, uint32_t multipleSignatureBase);
DatasetHeader* initDatasetHeader_MIT_AUC(DatasetGroupId datasetGroupId, DatasetId datasetId, char* version,
                                         bool multipleAlignmentFlag, bool byteOffsetSizeFlag, bool posOffsetIsUint40,
                                         bool nonOverlappingAURange, uint16_t sequencesCount, ReferenceId referenceId,
                                         uint8_t datasetType, uint8_t numClasses, uint8_t alphabetId,
                                         uint32_t numUClusters, bool uSignatureConstantLength, uint8_t uSignatureSize,
                                         uint8_t uSignatureLength, uint32_t numberUAccessUnits,
                                         uint32_t multipleSignatureBase, bool classContiguous);
DatasetHeader* initDatasetHeader_DSC(DatasetGroupId datasetGroupId, DatasetId datasetId, char* version,
                                     bool multipleAlignmentFlag, bool byteOffsetSizeFlag, bool posOffsetIsUint40,
                                     bool nonOverlappingAURange, uint16_t sequencesCount, ReferenceId referenceId,
                                     uint8_t datasetType, uint8_t numClasses, uint8_t alphabetId, uint32_t numUClusters,
                                     bool uSignatureConstantLength, uint8_t uSignatureSize, uint8_t uSignatureLength,
                                     uint32_t numberUAccessUnits, uint32_t multipleSignatureBase,
                                     bool orderedBlocksFlag);

void freeDatasetHeader(DatasetHeader* datasetHeader);
void setMITFlag(DatasetHeader* datasetHeader, bool value);
int setClassContiguousModeFlag(DatasetHeader* datasetHeader, bool classContiguousMode);
bool setSequenceId(DatasetHeader* datasetHeader, uint16_t sequenceIndex, SequenceID sequenceId);
bool setBlocksInSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t blocks);
bool setThresholdForSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t threshold);
bool getThresholdForSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex, uint32_t* threshold);
bool isMultipleAlignment(DatasetHeader* datasetHeader);
bool isPosOffsetUint40(DatasetHeader* datasetHeader);
bool isByteOffsetUint64(DatasetHeader* datasetHeader);
bool isBlockHeaderFlagSet(DatasetHeader* datasetHeader);
bool isUnmappedIndexingFlagSet(DatasetHeader* datasetHeader);
uint8_t getDatasetType(DatasetHeader* datasetHeader);
bool isOrderedByStartPosition(DatasetHeader* datasetHeader);
bool isNonOverlappingAURange(DatasetHeader* datasetHeader);
void setDatasetType(DatasetHeader* datasetHeader, uint8_t datasetType);
uint64_t getSizeContentDatasetHeader(DatasetHeader* datasetHeader);
uint64_t getSizeDatasetHeader(DatasetHeader* datasetHeader);
bool writeContentDatasetHeader(DatasetHeader* datasetHeader, FILE* outputFile);
bool writeDatasetHeader(DatasetHeader* datasetHeader, FILE* outputFile);
DatasetHeader* parseDatasetHeader(FILE* inputFile);
bool setNumberDescriptorsInClass(DatasetHeader* datasetHeader, unsigned int classId, uint8_t numberDescriptors);
void setClassType(DatasetHeader* datasetHeader, uint8_t classIt, ClassType classType);
void setDescriptorIdInClass(DatasetHeader* datasetHeader, uint8_t classIndex, uint8_t descriptorIndex,
                            uint8_t descriptorId);
uint8_t getNumberDescriptorsInClass(DatasetHeader* datasetHeader, uint8_t class_index);
uint8_t getDescriptorIdInClass(DatasetHeader* datasetHeader, uint8_t classId, uint8_t descriptorIndex);
void setConstantSignatureLength(DatasetHeader* datasetHeader, uint8_t uSignatureLength);
uint32_t getNumberUAccessUnits(DatasetHeader* datasetHeader);
uint16_t getSequencesCount(DatasetHeader* datasetHeader);
SequenceID getSequenceId(DatasetHeader* datasetHeader, uint16_t sequence_index);
bool getSequenceIndex(DatasetHeader* datasetHeader, SequenceID sequence_id, uint16_t* sequence_index);
uint8_t getClassesCount(DatasetHeader* datasetHeader);
bool hasClassType(DatasetHeader* datasetHeader, ClassType classType);
ClassType getClassType(DatasetHeader* datasetHeader, uint16_t class_i);
bool getClassIndexForType(DatasetHeader* datasetHeader, ClassType classType, uint8_t* class_i);
bool getDescriptorIndexForType(DatasetHeader* datasetHeader, ClassType classType, uint8_t descriptorId,
                               uint8_t* descriptorIndex);
uint32_t getUSignatureSize(DatasetHeader* datasetHeader);
uint32_t getBlocksInSequence(DatasetHeader* datasetHeader, uint16_t sequenceIndex);
uint32_t getMultipleSignatureBase(DatasetHeader* datasetHeader);
void setMultipleSignatureBase(DatasetHeader* datasetHeader, uint32_t newMultipleSignatureBase);
uint32_t getSignatureLength(DatasetHeader* datasetHeader);
size_t getDatasetHeaderSeekPosition(DatasetHeader* datasetHeader);
bool isMITFlagSet(DatasetHeader* datasetHeader);
ReferenceId getReferenceId(DatasetHeader* datasetHeader);

typedef struct {
    ByteArray* protection;
    StreamContainer* streamContainer;
    size_t seekPosition;
    bool hasSeek;
} StreamProtection;

StreamProtection* initStreamProtection(StreamContainer* streamContainer);
void freeStreamProtection(StreamProtection* streamProtection);
bool defineContentStreamProtection(StreamProtection* streamProtection, char* filename);
bool writeContentStreamProtection(StreamProtection* streamProtection, FILE* outputFile);
bool writeStreamProtection(StreamProtection* streamProtection, FILE* outputFile);
uint64_t getSizeContentStreamProtection(StreamProtection* streamProtection);
uint64_t getSizeStreamProtection(StreamProtection* streamProtection);
bool copyContentStreamProtection(StreamProtection* streamProtection, char* content, uint64_t contentSize);
StreamProtection* parseStreamProtection(StreamContainer* streamContainer, uint64_t boxContentSize, FILE* inputFile);
size_t getStreamProtectionSeekPosition(StreamProtection* streamProtection);

typedef struct {
    StreamContainer* streamContainer;
    uint8_t descriptorId;
    ClassType classId;
    uint16_t parametersSetId;
    uint32_t numberOfBlocks;
    uint8_t protectionFlag;
    size_t seekPosition;
    bool hasSeek;
} StreamHeader;

StreamHeader* initStreamHeader(StreamContainer* streamContainer, uint8_t descriptorId, ClassType classId,
                               uint32_t numberOfBlocks);
ClassType getStreamClass(StreamHeader* streamHeader);
uint8_t getStreamDescriptorId(StreamHeader* streamHeader);
bool writeContentStreamHeader(StreamHeader* streamHeader, FILE* outputFile);
bool writeStreamHeader(StreamHeader* streamHeader, FILE* outputFile);
uint64_t getSizeContentStreamHeader();
uint64_t getSizeStreamHeader();
StreamHeader* parseStreamHeader(StreamContainer* streamContainer, FILE* inputFile);
void freeStreamHeader(StreamHeader* streamHeader);
size_t getStreamHeaderSeekPosition(StreamHeader* streamHeader);

struct StreamContainer_ {
    StreamHeader* streamHeader;
    StreamProtection* streamProtection;
    Vector* datasFromFile;
    uint64_t seekPosition;
    bool hasSeek;
};

StreamContainer* initStreamContainer();

void freeStreamContainer(StreamContainer* streamContainer);
uint64_t getSizeContentStreamContainer(StreamContainer* streamContainer);
uint64_t getSizeStreamContainer(StreamContainer* streamContainer);
bool writeContentStreamContainer(StreamContainer* streamContainer, FILE* outputFile);
bool writeStreamContainer(StreamContainer* streamContainer, FILE* outputFile);
bool isStreamContainerValid(StreamContainer* streamContainer);
void setStreamHeader(StreamContainer* streamContainer, StreamHeader* streamHeader);
void setStreamProtection(StreamContainer* streamContainer, StreamProtection* streamProtection);
void setDataFromFilename(StreamContainer* streamContainer, char* filename);
void setDataFromFilenameWithBoundaries(StreamContainer* streamContainer, char* filename, uint64_t startPos,
                                       uint64_t endPos);
StreamContainer* parseStreamContainer(uint64_t boxContentSize, FILE* inputFile, char* fileName,
                                      DatasetContainer* datasetContainer);
Vector* getDataFromStreamContainer(StreamContainer* streamContainer);
uint64_t getStreamContainerSeekPosition(StreamContainer* streamContainer);
bool generateStreamSeekPoints(StreamContainer* streamContainer);
bool addBlockToStream(StreamContainer* streamContainer, FromFile* block);

typedef struct AccessUnitProtection_ AccessUnitProtection;
typedef struct AccessUnitInformation_ AccessUnitInformation;
typedef struct Block_ Block;
struct AccessUnitContainer_ {
    AccessUnitHeader* accessUnitHeader;
    AccessUnitInformation* accessUnitInformation;
    AccessUnitProtection* accessUnitProtection;
    long seekPosition;
    Vector* blocks;
    DatasetContainer* datasetContainer;
};
AccessUnitContainer* initAccessUnitContainer(DatasetContainer* datasetContainer);
void freeAccessUnitContainer(AccessUnitContainer* accessUnitContainer);
bool isAccessUnitContainerValid(AccessUnitContainer* accessUnitContainer);
uint64_t getAccessUnitContainerContentSize(AccessUnitContainer* accessUnitContainer);
uint64_t getAccessUnitContainerSize(AccessUnitContainer* accessUnitContainer);
bool writeAccessUnitContainer(FILE* outputFile, AccessUnitContainer* accessUnitContainer);
AccessUnitHeader* getAccessUnitHeader(AccessUnitContainer* accessUnitContainer);
void setAccessUnitContainerHeader(AccessUnitContainer* accessUnitContainer, AccessUnitHeader* accessUnitHeader);
void setAccessUnitContainerProtection(AccessUnitContainer* accessUnitContainer,
                                      AccessUnitProtection* accessUnitProtection);
void setAccessUnitContainerInformation(AccessUnitContainer* accessUnitContainer,
                                       AccessUnitInformation* accessUnitInformation);
AccessUnitContainer* parseAccessUnitContainer(uint64_t boxContentSize, FILE* inputFile, char* fileName,
                                              DatasetContainer* datasetContainer);
long getAccessUnitContainerSeekPoint(AccessUnitContainer* accessUnitContainer);
void addBlock(AccessUnitContainer* accessUnitContainer, Block* block);
Vector* getBlocks(AccessUnitContainer* accessUnitContainer);

struct AccessUnitHeader_ {
    DatasetContainer* datasetContainer;
    uint32_t access_unit_ID;
    uint8_t num_blocks;
    uint8_t parameter_set_ID;
    ClassType au_type;
    uint32_t reads_count;
    uint16_t mm_threshold;
    uint32_t mm_count;
    long seekPosition;

    // if dataset_type == 2
    SequenceID ref_sequence_id;
    uint64_t ref_start_position;
    uint64_t ref_end_position;

    // if MIT_flag == 0
    // if AU_type != U_TYPE_AU || dataset_type == 2
    SequenceID sequence_ID;
    uint64_t AU_start_position;
    uint64_t AU_end_position;
    // if multiple_alignment_flag
    uint64_t extended_AU_start_position;
    uint64_t extended_AU_end_position;
    // else
    Signatures* signatures;
};
AccessUnitHeader* initAccessUnitHeader();
AccessUnitHeader* initAccessUnitHeaderWithValues(DatasetContainer* datasetContainer, uint32_t access_unit_ID,
                                                 uint8_t num_blocks, uint8_t parameter_set_ID, ClassType au_type,
                                                 uint32_t reads_count, uint16_t mm_threshold, uint32_t mm_count);
void freeAccessUnitHeader(AccessUnitHeader* accessUnitHeader);
uint64_t getAccessUnitHeaderContentSize(AccessUnitHeader* accessUnitHeader);
uint64_t getAccessUnitHeaderSize(AccessUnitHeader* accessUnitHeader);
bool writeAccessUnitHeaderContent(FILE* outputFile, AccessUnitHeader* accessUnitHeader);
bool writeAccessUnitHeader(FILE* outputFile, AccessUnitHeader* accessUnitHeader);
AccessUnitHeader* parseAccessUnitHeader(FILE* inputFile, DatasetContainer* datasetContainer);
long getAccessUnitHeaderSeekPosition(AccessUnitHeader* accessUnitHeader);
uint8_t getNumBlocks(AccessUnitHeader* accessUnitHeader);
uint32_t getAccessUnitId(AccessUnitHeader* accessUnitHeader);
uint16_t getParametersSetId(AccessUnitHeader* accessUnitHeader);
uint32_t getReadsCount(AccessUnitHeader* accessUnitHeader);
SequenceID getReferenceSequence(AccessUnitHeader* accessUnitHeader);
uint64_t getReferenceStart(AccessUnitHeader* accessUnitHeader);
uint64_t getReferenceEnd(AccessUnitHeader* accessUnitHeader);
uint16_t getMMThreshold(AccessUnitHeader* accessUnitHeader);
uint32_t getMMCount(AccessUnitHeader* accessUnitHeader);
ClassType getAccessUnitType(AccessUnitHeader* accessUnitHeader);
SequenceID getAccessUnitSequenceID(AccessUnitHeader* accessUnitHeader);
uint64_t getAccessUnitStart(AccessUnitHeader* accessUnitHeader);
uint64_t getAccessUnitEnd(AccessUnitHeader* accessUnitHeader);
uint64_t getExtendedAccessUnitStart(AccessUnitHeader* accessUnitHeader);
uint64_t getExtendedAccessUnitEnd(AccessUnitHeader* accessUnitHeader);
int setAccessUnitHeaderSequence_ID(AccessUnitHeader* accessUnitHeader, SequenceID sequenceId);
int setAccessUnitHeaderAuStartPosition(AccessUnitHeader* accessUnitHeader, uint64_t auStartPosition);
int setAccessUnitHeaderAuEndPosition(AccessUnitHeader* accessUnitHeader, uint64_t auEndPosition);
int setAccessUnitHeaderAuExtendedStartPosition(AccessUnitHeader* accessUnitHeader, uint64_t auExtendedStartPosition);
int setAccessUnitHeaderAuExtendedEndPosition(AccessUnitHeader* accessUnitHeader, uint64_t auExtendedEndPosition);
int setAccessUnitHeaderReferenceSequence_ID(AccessUnitHeader* accessUnitHeader, SequenceID referenceSequenceId);
int setAccessUnitHeaderReferenceStartPosition(AccessUnitHeader* accessUnitHeader, uint64_t referenceStartPosition);
int setAccessUnitHeaderReferenceEndPosition(AccessUnitHeader* accessUnitHeader, uint64_t referenceEndPosition);

struct AccessUnitProtection_ {
    ByteArray* protection;
    long seekPosition;
};
AccessUnitProtection* initAccessUnitProtection();
void freeAccessUnitProtection(AccessUnitProtection* accessUnitProtection);
bool defineContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection, char* filename);
bool writeContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection, FILE* outputFile);
bool writeAccessUnitProtection(AccessUnitProtection* accessUnitProtection, FILE* outputFile);
uint64_t getSizeContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection);
uint64_t getSizeAccessUnitProtection(AccessUnitProtection* accessUnitProtection);
AccessUnitProtection* parseAccessUnitProtection(uint64_t boxContentSize, FILE* inputFile);
bool copyContentAccessUnitProtection(AccessUnitProtection* accessUnitProtection, char* content, uint64_t contentSize);
long getAccessUnitProtectionSeekPosition(AccessUnitProtection* accessUnitInformation);

struct AccessUnitInformation_ {
    ByteArray* information;
    long seekPosition;
};
AccessUnitInformation* initAccessUnitInformation();
void freeAccessUnitInformation(AccessUnitInformation* accessUnitInformation);
bool defineContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation, char* filename);
bool writeContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation, FILE* outputFile);

bool writeAccessUnitInformation(FILE* outputFile, AccessUnitInformation* accessUnitInformation);
uint64_t getSizeContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation);
uint64_t getSizeAccessUnitInformation(AccessUnitInformation* accessUnitInformation);
AccessUnitInformation* parseAccessUnitInformation(uint64_t boxContentSize, FILE* inputFile);
bool copyContentAccessUnitInformation(AccessUnitInformation* accessUnitInformation, char* content,
                                      uint64_t contentSize);
long getAccessUnitInformationSeekPosition(AccessUnitInformation* accessUnitInformation);

typedef struct {
    DatasetContainer* datasetContainer;
    uint8_t descriptorId;
    uint32_t payloadSize;
} BlockHeader;

BlockHeader* initBlockHeader(DatasetContainer* datasetContainer, uint8_t descriptorId, uint32_t payloadSize);
uint8_t getBlocksDescriptorId(BlockHeader* blockHeader);
uint32_t getPayloadSize(BlockHeader* blockHeader);
BlockHeader* parseBlockHeader(DatasetContainer* datasetContainer, FILE* inputFile);
bool writeBlockHeader(BlockHeader* blockHeader, FILE* outputFile);
bool writeBlockHeaderDataUnit(BlockHeader* blockHeader, FILE* outputFile);
void freeBlockHeader(BlockHeader* blockHeader);
uint64_t getBlockHeaderSize(BlockHeader* blockHeader);

struct Block_ {
    BlockHeader* blockHeader;
    FromFile* payload;
    uint32_t padding_size;
    DatasetContainer* datasetContainer;
};

void freeBlock(Block* block);

Block* initBlock(DatasetContainer* datasetContainer, FromFile* fromFile);
Block* initBlockWithHeader(uint8_t descriptorId, uint32_t payloadSize, FromFile* payload);
void setBlockHeader(Block* block, BlockHeader* blockHeader);
void setPaddingSize(Block* block, uint32_t paddingSize);
bool writeBlock(Block* block, FILE* outputFile);
Block* parseBlockContainerAUCmode(DatasetContainer* datasetContainer, FILE* inputFile, char* fileName);
uint64_t getBlockSize(Block* block);

typedef struct {
    uint16_t parent_parameter_setId;
    uint16_t parameter_setId;

    Encoding_ParametersType* encodingParameters;
} DataUnitParametersSet;
bool parseDataUnitsParametersSet(DataUnitParametersSet** parametersSet, FILE* inputFile, uint32_t sizeContent);
bool writeParametersSet(DataUnitParametersSet* parametersSet, FILE* outputFile);

#endif  // MPEGG_CAPSULATOR_BOXES_H
