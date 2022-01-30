#ifndef GENIE_C_API_H
#define GENIE_C_API_H

extern "C" {

#include <stdint.h>

enum genie_ReturnCode : uint8_t {
    genie_ReturnCode_G_SUCCESS = 0,
    genie_ReturnCode_G_PARTIALLY_AUTHORIZED = 1,
    genie_ReturnCode_G_NOT_AUTHORIZED = 2,
    genie_ReturnCode_G_VERIFICATION_FAILED = 3,
    genie_ReturnCode_G_DECRYPTION_FAILED = 4,
    genie_ReturnCode_G_DATASETGROUP_NOTFOUND = 5,
    genie_ReturnCode_G_DATASET_NOTFOUND = 6,
    genie_ReturnCode_G_ACCESSUNITS_NOTFOUND = 7,
    genie_ReturnCode_G_REFERENCE_NOTFOUND = 8,
    genie_ReturnCode_G_SEQUENCE_NOTFOUND = 9,
    genie_ReturnCode_G_METADATA_FIELD_NOTFOUND = 10,
    genie_ReturnCode_G_INVALID_METADATA = 11,
    genie_ReturnCode_G_INVALID_REFERENCE = 12,
    genie_ReturnCode_G_INVALID_PARAMETER = 13,
    genie_ReturnCode_G_INVALID_BITSTREAM = 14,
    genie_ReturnCode_G_UNLISTED_ERROR = 15
};

const char* genie_strerror(genie_ReturnCode rc);

enum genie_SegmentType : uint8_t {
    genie_SegmentType_SINGLE = 0,
    genie_SegmentType_FIRST = 1,
    genie_SegmentType_SECOND = 2,
    genie_SegmentType_COUNT = 3
};

enum genie_StatisticsIndex : uint8_t {
    genie_StatisticsIndex_MINIMUM = 0,
    genie_StatisticsIndex_MAXIMUM = 1,
    genie_StatisticsIndex_AVERAGE = 2,
    genie_StatisticsIndex_COUNT = 3
};

enum genie_Strand : uint8_t {
    genie_Strand_UNMAPPED_UNKNOWN = 0,
    genie_Strand_FORWARD = 1,
    genie_Strand_REVERSE = 2,
    genie_Strand_COUNT = 3
};

enum genie_StrandStrict : uint8_t {
    genie_StrandStrict_FORWARD = 0,
    genie_StrandStrict_REVERSE = 1,
    genie_StrandStrict_COUNT = 2
};

enum genie_StrandPaired : uint8_t {
    genie_StrandPaired_UNMAPPED_UNMAPPED = 0,
    genie_StrandPaired_UNMAPPED_FORWARD = 1,
    genie_StrandPaired_UNMAPPED_REVERSE = 2,
    genie_StrandPaired_FORWARD_UNMAPPED = 3,
    genie_StrandPaired_REVERSE_UNMAPPED = 4,
    genie_StrandPaired_FORWARD_FORWARD = 5,
    genie_StrandPaired_FORWARD_REVERSE = 6,
    genie_StrandPaired_REVERSE_FORWARD = 7,
    genie_StrandPaired_REVERSE_REVERSE = 8,
    genie_StrandPaired_COUNT = 9
};

enum genie_ClipType : uint8_t { genie_ClipType_SOFT = 0, genie_ClipType_HARD = 1, genie_ClipType_COUNT = 2 };

enum genie_ClipTypeCombination : uint8_t {
    genie_ClipTypeCombination_NONE = 0,
    genie_ClipTypeCombination_SOFT = 1,
    genie_ClipTypeCombination_SOFT_HARD = 2,
    genie_ClipTypeCombination_COUNT = 3
};

enum genie_PhredBins {
    genie_PhredBins_0_10 = 0,
    genie_PhredBins_10_25 = 1,
    genie_PhredBins_25_50 = 2,
    genie_PhredBins_50_75 = 3,
    genie_PhredBins_75_90 = 4,
    genie_PhredBins_90_100 = 5,
    genie_PhredBins_COUNT = 6
};

enum genie_TagType : uint8_t {
    genie_TagType_INT32 = 0,
    genie_TagType_STRING = 1,
    genie_TagType_UINT8 = 2,
    genie_TagType_INT8 = 3,
    genie_TagType_UINT16 = 4,
    genie_TagType_INT16 = 5,
    genie_TagType_UINT32 = 6,
    genie_TagType_HEX4 = 7,
    genie_TagType_FLOAT32 = 8,
    genie_TagType_FLOAT64 = 9
};

enum genie_RecordClass : uint8_t {
    genie_RecordClass_P = 0,
    genie_RecordClass_N = 1,
    genie_RecordClass_M = 2,
    genie_RecordClass_I = 3,
    genie_RecordClass_HM = 4,
    genie_RecordClass_U = 5,
    genie_RecordClass_COUNT = 6,
};

enum genie_AlphabetACGNT : uint8_t {
    genie_AlphabetACGNT_A = 0,
    genie_AlphabetACGNT_C = 1,
    genie_AlphabetACGNT_G = 2,
    genie_AlphabetACGNT_N = 3,
    genie_AlphabetACGNT_T = 4,
    genie_AlphabetACGNT_COUNT = 5,
};

struct genie_Hierarchy {
    uint64_t datasetGroupsCount;
    uint64_t* datasetGroupID;  // [datasetGroupsCount]
    uint64_t* datasetsCount;   // [datasetGroupsCount]
    uint64_t** datasetID;      // [datasetGroupsCount][datasetsCount[i]]
};

// TODO implement
struct genie_mpeggRecord {
    void* tmp;
};

struct genie_GenTag {
    char key[2];
    genie_TagType type;
    uint16_t length;
    union {
        int32_t i32;
        char* string;
        uint8_t u8;
        int8_t i8;
        uint16_t u16;
        int16_t i16;
        uint32_t u32;
        uint8_t hex;
        float f32;
        double f64;
    } value;
};

struct genie_GenAux {
    uint8_t numberOfTags;
    genie_GenTag* auxFields;  // [numberOfTags]
};

struct genie_genAuxRecord {
    uint8_t numberOfGenAux;
    genie_GenAux* auxSet;  // [numberOfGenAux]
};

struct genie_Records {
    uint64_t datasetGroupID;
    uint64_t datasetID;
    uint64_t recordsCount;
    genie_mpeggRecord* records;   // [recordsCount]
    genie_genAuxRecord* auxInfo;  // [recordsCount]
};

struct genie_Reference {
    uint64_t seqCount;
    char** seqName;      // [seqCount] \0
    uint64_t* seqStart;  // [seqCount]
    uint64_t* seqEnd;    // [seqCount]
    char** refSequence;  // [seqCount] \0
};

struct genie_RegionProtection {
    char* sequenceName;  // \0
    uint64_t startPos;
    uint64_t endPos;
    uint64_t classID;
    uint64_t numKeys;
    char** keyName;  // [numKeys] \0
};

struct genie_SimpleSegmentStatistics {
    uint64_t readsNumber;
    uint64_t* segmentsNumberReadsDistribution;  // [max_segments]
    uint64_t qualityCheckFailedReadsNumber;
    uint64_t segmentLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t mappedStrandSegmentDistribution[genie_Strand::genie_Strand_COUNT];
    uint64_t properlyPairedNumber;
    uint64_t mappedStrandPairDistribution[genie_StrandPaired::genie_StrandPaired_COUNT];
    uint64_t maxAlignments;
    uint64_t* multipleAlignmentSegmentDistribution;  // [maxAlignments + 1]
    uint64_t coverage[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t weightedCoverage[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t errorsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t substitutionsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t insertionsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t insertionsLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t deletionsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t deletionsLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t splicesNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t splicesLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t alignmentScore[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];
    uint64_t classSegmentDistribution[genie_RecordClass::genie_RecordClass_COUNT];
    uint64_t clippedSegmentDistribution[genie_ClipTypeCombination::genie_ClipTypeCombination_COUNT];
    uint64_t opticalDuplicatesNumber;
    uint64_t chimerasNumber;
};

struct genie_ExtendedSegmentStatistics {
    uint64_t* mappedBasesNumberDistribution;    // [segmentLength[1] + 1]
    uint64_t* coverage;                         // [endPos – startPos + 1]
    uint64_t* weightedCoverage;                 // [endPos – startPos + 1]
    uint64_t* errorsNumberDistribution;         // [errorsNumber[1] + 1]
    uint64_t* errorsPositionDistribution;       // [segmentLength[1]]
    uint64_t* substitutionsNumberDistribution;  // [substitutionsNumber[1] + 1]
    uint64_t*
        substitutionsTransitionDistribution[genie_AlphabetACGNT::genie_AlphabetACGNT_COUNT]
                                           [genie_AlphabetACGNT::genie_AlphabetACGNT_COUNT];  // [segmentLength[1]]
    uint64_t* substitutionsPositionDistribution;                                              // [segmentLength[1]]
    uint64_t* insertionsNumberDistribution;                                   // [insertionsNumber[1] + 1]
    uint64_t* insertionsLengthDistribution;                                   // [insertionsLength[1] + 1]
    uint64_t* insertionsPositionDistribution;                                 // [segmentLength[1]]
    uint64_t* deletionsNumberDistribution;                                    // [deletionsNumber[1] + 1]
    uint64_t* deletionsLengthDistribution;                                    // [deletionsLength[1] + 1]
    uint64_t* deletionsPositionDistribution;                                  // [segmentLength[1]]
    uint64_t* splicesNumberDistribution[genie_Strand::genie_Strand_COUNT];    // [splicesNumber[1] + 1],
    uint64_t* splicesLengthDistribution[genie_Strand::genie_Strand_COUNT];    // [splicesLength[1] + 1],
    uint64_t* splicesPositionDistribution[genie_Strand::genie_Strand_COUNT];  // [segmentLength[1]],
    uint64_t* alignmentScoreValueDistribution;                                // [alignmentScore[1] + 1]
    uint64_t* alignmentScoreSegmentLengthDistribution
        [genie_StatisticsIndex::genie_StatisticsIndex_COUNT];                             // [segmentLength[1]],
    uint64_t* basesPositionDistribution[genie_AlphabetACGNT::genie_AlphabetACGNT_COUNT];  // [segmentLength[1]]
    uint64_t gcContentValueDistribution[101];                                             // GC content in percent 0-100
    uint64_t maxQualityScore;
    uint64_t* qualityScoreDistribution;  // [maxQualityScore + 1]
    uint64_t*
        qualityScorePositionDistribution[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];  // [segmentLength[1]]
    uint64_t*
        qualityScorePositionPercentilesDistribution[genie_PhredBins::genie_PhredBins_COUNT];  // [segmentLength[1]],
};

struct genie_AdvancedSegmentStatistics {
    genie_SimpleSegmentStatistics simpleStatistics[genie_StrandStrict::genie_StrandStrict_COUNT];
    genie_ExtendedSegmentStatistics extendedStatistics[genie_StrandStrict::genie_StrandStrict_COUNT];
};

struct genie_SimpleFilter {
    uint64_t numberOfGroups;
    char** groupNames;  // [numberOfGroups] \0
    bool classID[genie_RecordClass::genie_RecordClass_COUNT];
    uint64_t sequenceID;
    uint64_t startPos;
    uint64_t endPos;
    bool singleEndsStrand[genie_Strand::genie_Strand_COUNT];
    bool pairedEndsStrand[genie_StrandPaired::genie_StrandPaired_COUNT];
    bool includeClippedReads[genie_ClipType::genie_ClipType_COUNT];  // genie_clips
    bool includeMultipleAlignments;
    bool includeOpticalDuplicates;
    bool includeQualityCheckFailed;
    bool includeAuxRecords;
    bool includeReadNames;
    bool includeQualityValues;
    bool mismatchesIncludeNs;
};

struct genie_RangeFloat {
    float min;
    float max;
};

struct genie_RangeInt {
    uint64_t min;
    uint64_t max;
};

struct genie_SegmentFilter {
    bool filterScope[genie_SegmentType::genie_SegmentType_COUNT];
    genie_RangeInt mappedBasesRange;
    genie_RangeFloat mappedFractionRange;
    genie_RangeInt errorsRange;
    genie_RangeFloat errorsFractionRange;
    genie_RangeInt substitutionsRange;
    genie_RangeFloat substitutionsFractionRange;
    genie_RangeInt insertionsRange;
    genie_RangeFloat insertionsFractionRange;
    genie_RangeInt insertionsLengthRange;
    genie_RangeFloat insertionsLengthFractionRange;
    genie_RangeInt deletionsRange;
    genie_RangeFloat deletionsFractionRange;
    genie_RangeInt deletionsLengthRange;
    genie_RangeFloat deletionsLengthFractionRange;
    genie_RangeInt splicesRange;
    genie_RangeFloat splicesFractionRange;
    genie_RangeInt splicesLengthRange;
    int splicesDirectionAsEnd;
    genie_RangeFloat alignmentScoreRange;
    genie_RangeInt qualityScoreRange;
};

struct genie_AdvancedFilter {
    genie_SimpleFilter filter;
    uint64_t segmentFiltersCount;
    genie_SegmentFilter* segmentFilters;  // [segmentFiltersCount]
};

genie_ReturnCode genie_getHierarchy(genie_Hierarchy** outputHierarchy);

genie_ReturnCode genie_getDataBySimpleFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                             const genie_SimpleFilter* filter, genie_Records** outputRecords);

genie_ReturnCode genie_getDataByAdvancedFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                               const genie_AdvancedFilter* filter, genie_Records** outputRecords);

genie_ReturnCode genie_getDataBySignature(uint64_t datasetGroupID, uint64_t datasetID, const char* signature,
                                          genie_Records** outputRecords);

genie_ReturnCode genie_getDataByLabel(uint64_t datasetGroupID, const char* labelID, genie_Records** outputRecords);

genie_ReturnCode genie_getMetadataFields(uint64_t datasetGroupID, uint64_t datasetID, char*** outputMetadata);

genie_ReturnCode genie_getMetadataContent(uint64_t datasetGroupID, uint64_t datasetID, const char* fieldName,
                                          char** outputMetadata);

genie_ReturnCode genie_getDatasetGroupProtection(uint64_t datasetGroupID, char** outputProtection);

genie_ReturnCode genie_getDatasetProtection(uint64_t datasetGroupID, uint64_t datasetID, char** outputProtection);

genie_ReturnCode genie_getDatasetRegionProtection(uint64_t datasetGroup_ID, uint64_t datasetID, uint64_t sequenceID,
                                                  uint64_t startPos, uint64_t endPos,
                                                  genie_RegionProtection** outputProtection);

genie_ReturnCode genie_getDatasetReference(uint64_t datasetGroupID, uint64_t datasetID, bool includeSequences,
                                           genie_Reference* outputReference);

genie_ReturnCode genie_getSimpleStatistics(uint64_t datasetGroupID, uint64_t datasetID, uint64_t sequenceID,
                                           uint64_t startPos, uint64_t endPos, uint64_t* maxSegments,
                                           genie_SimpleSegmentStatistics** outputStatistics);

genie_ReturnCode genie_getAdvancedStatistics(uint64_t datasetGroupID, uint64_t datasetID, uint64_t sequenceID,
                                             uint64_t startPos, uint64_t endPos, uint64_t* maxSegments,
                                             genie_AdvancedSegmentStatistics** outputStatistics);
}
#endif  // GENIE_C_API_H
