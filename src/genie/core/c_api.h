/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_C_API_H
#define GENIE_C_API_H

// ---------------------------------------------------------------------------------------------------------------------

extern "C" {

// ---------------------------------------------------------------------------------------------------------------------

#include <stdint.h>

/**
 * @brief
 */
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

/**
 * @brief
 * @param rc
 * @return
 */
const char* genie_strerror(genie_ReturnCode rc);

/**
 * @brief
 */
enum genie_SegmentType : uint8_t {
    genie_SegmentType_SINGLE = 0,
    genie_SegmentType_FIRST = 1,
    genie_SegmentType_SECOND = 2,
    genie_SegmentType_COUNT = 3
};

/**
 * @brief
 */
enum genie_StatisticsIndex : uint8_t {
    genie_StatisticsIndex_MINIMUM = 0,
    genie_StatisticsIndex_MAXIMUM = 1,
    genie_StatisticsIndex_AVERAGE = 2,
    genie_StatisticsIndex_COUNT = 3
};

/**
 * @brief
 */
enum genie_Strand : uint8_t {
    genie_Strand_UNMAPPED_UNKNOWN = 0,
    genie_Strand_FORWARD = 1,
    genie_Strand_REVERSE = 2,
    genie_Strand_COUNT = 3
};

/**
 * @brief
 */
enum genie_StrandStrict : uint8_t {
    genie_StrandStrict_FORWARD = 0,
    genie_StrandStrict_REVERSE = 1,
    genie_StrandStrict_COUNT = 2
};

/**
 * @brief
 */
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

/**
 * @brief
 */
enum genie_ClipType : uint8_t { genie_ClipType_SOFT = 0, genie_ClipType_HARD = 1, genie_ClipType_COUNT = 2 };

/**
 * @brief
 */
enum genie_ClipTypeCombination : uint8_t {
    genie_ClipTypeCombination_NONE = 0,
    genie_ClipTypeCombination_SOFT = 1,
    genie_ClipTypeCombination_SOFT_HARD = 2,
    genie_ClipTypeCombination_COUNT = 3
};

/**
 * @brief
 */
enum genie_PhredBins {
    genie_PhredBins_0_10 = 0,
    genie_PhredBins_10_25 = 1,
    genie_PhredBins_25_50 = 2,
    genie_PhredBins_50_75 = 3,
    genie_PhredBins_75_90 = 4,
    genie_PhredBins_90_100 = 5,
    genie_PhredBins_COUNT = 6
};

/**
 * @brief
 */
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

/**
 * @brief
 */
enum genie_RecordClass : uint8_t {
    genie_RecordClass_P = 0,
    genie_RecordClass_N = 1,
    genie_RecordClass_M = 2,
    genie_RecordClass_I = 3,
    genie_RecordClass_HM = 4,
    genie_RecordClass_U = 5,
    genie_RecordClass_COUNT = 6,
};

/**
 * @brief
 */
enum genie_AlphabetACGNT : uint8_t {
    genie_AlphabetACGNT_A = 0,
    genie_AlphabetACGNT_C = 1,
    genie_AlphabetACGNT_G = 2,
    genie_AlphabetACGNT_N = 3,
    genie_AlphabetACGNT_T = 4,
    genie_AlphabetACGNT_COUNT = 5,
};

/**
 * @brief
 */
struct genie_Hierarchy {
    uint64_t datasetGroupsCount;  //!< @brief
    uint64_t* datasetGroupID;     //!< @brief [datasetGroupsCount]
    uint64_t* datasetsCount;      //!< @brief [datasetGroupsCount]
    uint64_t** datasetID;         //!< @brief [datasetGroupsCount][datasetsCount[i]]
};

/**
 * @brief
 */
struct genie_mpeggRecord {
    void* tmp;
    // TODO implement
};

/**
 * @brief
 */
struct genie_GenTag {
    char key[2];         //!< @brief
    genie_TagType type;  //!< @brief
    uint16_t length;     //!< @brief
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
    } value;  //!< @brief
};

/**
 * @brief
 */
struct genie_GenAux {
    uint8_t numberOfTags;     //!< @brief
    genie_GenTag* auxFields;  //!< @brief [numberOfTags]
};

/**
 * @brief
 */
struct genie_genAuxRecord {
    uint8_t numberOfGenAux;  //!< @brief
    genie_GenAux* auxSet;    //!< @brief [numberOfGenAux]
};

/**
 * @brief
 */
struct genie_Records {
    uint64_t datasetGroupID;      //!< @brief
    uint64_t datasetID;           //!< @brief
    uint64_t recordsCount;        //!< @brief
    genie_mpeggRecord* records;   //!< @brief [recordsCount]
    genie_genAuxRecord* auxInfo;  //!< @brief [recordsCount]
};

/**
 * @brief
 */
struct genie_Reference {
    uint64_t seqCount;   //!< @brief
    char** seqName;      //!< @brief [seqCount] \0
    uint64_t* seqStart;  //!< @brief [seqCount]
    uint64_t* seqEnd;    //!< @brief [seqCount]
    char** refSequence;  //!< @brief [seqCount] \0
};

/**
 * @brief
 */
struct genie_RegionProtection {
    char* sequenceName;  //!< @brief \0
    uint64_t startPos;   //!< @brief
    uint64_t endPos;     //!< @brief
    uint64_t classID;    //!< @brief
    uint64_t numKeys;    //!< @brief
    char** keyName;      //!< @brief [numKeys] \0
};

/**
 * @brief
 */
struct genie_SimpleSegmentStatistics {
    uint64_t readsNumber;                                                                 //!< @brief
    uint64_t* segmentsNumberReadsDistribution;                                            //!< @brief [max_segments]
    uint64_t qualityCheckFailedReadsNumber;                                               //!< @brief
    uint64_t segmentLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];           //!< @brief
    uint64_t mappedStrandSegmentDistribution[genie_Strand::genie_Strand_COUNT];           //!< @brief
    uint64_t properlyPairedNumber;                                                        //!< @brief
    uint64_t mappedStrandPairDistribution[genie_StrandPaired::genie_StrandPaired_COUNT];  //!< @brief
    uint64_t maxAlignments;                                                               //!< @brief
    uint64_t* multipleAlignmentSegmentDistribution;                                    //!< @brief [maxAlignments + 1]
    uint64_t coverage[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];             //!< @brief
    uint64_t weightedCoverage[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];     //!< @brief
    uint64_t errorsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];         //!< @brief
    uint64_t substitutionsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];  //!< @brief
    uint64_t insertionsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];     //!< @brief
    uint64_t insertionsLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];     //!< @brief
    uint64_t deletionsNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];      //!< @brief
    uint64_t deletionsLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];      //!< @brief
    uint64_t splicesNumber[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];        //!< @brief
    uint64_t splicesLength[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];        //!< @brief
    uint64_t alignmentScore[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];       //!< @brief
    uint64_t classSegmentDistribution[genie_RecordClass::genie_RecordClass_COUNT];     //!< @brief
    uint64_t clippedSegmentDistribution[genie_ClipTypeCombination::genie_ClipTypeCombination_COUNT];  //!< @brief
    uint64_t opticalDuplicatesNumber;                                                                 //!< @brief
    uint64_t chimerasNumber;                                                                          //!< @brief
};

/**
 * @brief
 */
struct genie_ExtendedSegmentStatistics {
    uint64_t* mappedBasesNumberDistribution;    //!< @brief [segmentLength[1] + 1]
    uint64_t* coverage;                         //!< @brief [endPos – startPos + 1]
    uint64_t* weightedCoverage;                 //!< @brief [endPos – startPos + 1]
    uint64_t* errorsNumberDistribution;         //!< @brief [errorsNumber[1] + 1]
    uint64_t* errorsPositionDistribution;       //!< @brief [segmentLength[1]]
    uint64_t* substitutionsNumberDistribution;  //!< @brief [substitutionsNumber[1] + 1]
    uint64_t*
        substitutionsTransitionDistribution[genie_AlphabetACGNT::genie_AlphabetACGNT_COUNT]
                                           [genie_AlphabetACGNT::genie_AlphabetACGNT_COUNT];  //!< @brief
                                                                                              //!< [segmentLength[1]]
    uint64_t* substitutionsPositionDistribution;                              //!< @brief [segmentLength[1]]
    uint64_t* insertionsNumberDistribution;                                   //!< @brief [insertionsNumber[1] + 1]
    uint64_t* insertionsLengthDistribution;                                   //!< @brief [insertionsLength[1] + 1]
    uint64_t* insertionsPositionDistribution;                                 //!< @brief [segmentLength[1]]
    uint64_t* deletionsNumberDistribution;                                    //!< @brief [deletionsNumber[1] + 1]
    uint64_t* deletionsLengthDistribution;                                    //!< @brief [deletionsLength[1] + 1]
    uint64_t* deletionsPositionDistribution;                                  //!< @brief [segmentLength[1]]
    uint64_t* splicesNumberDistribution[genie_Strand::genie_Strand_COUNT];    //!< @brief [splicesNumber[1] + 1],
    uint64_t* splicesLengthDistribution[genie_Strand::genie_Strand_COUNT];    //!< @brief [splicesLength[1] + 1],
    uint64_t* splicesPositionDistribution[genie_Strand::genie_Strand_COUNT];  //!< @brief [segmentLength[1]],
    uint64_t* alignmentScoreValueDistribution;                                //!< @brief [alignmentScore[1] + 1]
    uint64_t* alignmentScoreSegmentLengthDistribution
        [genie_StatisticsIndex::genie_StatisticsIndex_COUNT];  //!< @brief [segmentLength[1]],
    uint64_t* basesPositionDistribution[genie_AlphabetACGNT::genie_AlphabetACGNT_COUNT];  //!< @brief [segmentLength[1]]
    uint64_t gcContentValueDistribution[101];  //!< @brief GC content in percent 0-100
    uint64_t maxQualityScore;                  //!< @brief
    uint64_t* qualityScoreDistribution;        //!< @brief [maxQualityScore + 1]
    uint64_t*
        qualityScorePositionDistribution[genie_StatisticsIndex::genie_StatisticsIndex_COUNT];  //!< @brief
                                                                                               //!< [segmentLength[1]]
    uint64_t*
        qualityScorePositionPercentilesDistribution[genie_PhredBins::genie_PhredBins_COUNT];  //!< @brief
                                                                                              //!< [segmentLength[1]],
};

/**
 * @brief
 */
struct genie_AdvancedSegmentStatistics {
    genie_SimpleSegmentStatistics simpleStatistics[genie_StrandStrict::genie_StrandStrict_COUNT];      //!< @brief
    genie_ExtendedSegmentStatistics extendedStatistics[genie_StrandStrict::genie_StrandStrict_COUNT];  //!< @brief
};

/**
 * @brief
 */
struct genie_SimpleFilter {
    uint64_t numberOfGroups;                                              //!< @brief
    char** groupNames;                                                    //!< @brief [numberOfGroups] \0
    bool classID[genie_RecordClass::genie_RecordClass_COUNT];             //!< @brief
    uint64_t sequenceID;                                                  //!< @brief
    uint64_t startPos;                                                    //!< @brief
    uint64_t endPos;                                                      //!< @brief
    bool singleEndsStrand[genie_Strand::genie_Strand_COUNT];              //!< @brief
    bool pairedEndsStrand[genie_StrandPaired::genie_StrandPaired_COUNT];  //!< @brief
    bool includeClippedReads[genie_ClipType::genie_ClipType_COUNT];       //!< @brief genie_clips
    bool includeMultipleAlignments;                                       //!< @brief
    bool includeOpticalDuplicates;                                        //!< @brief
    bool includeQualityCheckFailed;                                       //!< @brief
    bool includeAuxRecords;                                               //!< @brief
    bool includeReadNames;                                                //!< @brief
    bool includeQualityValues;                                            //!< @brief
    bool mismatchesIncludeNs;                                             //!< @brief
};

/**
 * @brief
 */
struct genie_RangeFloat {
    float min;  //!< @brief
    float max;  //!< @brief
};

/**
 * @brief
 */
struct genie_RangeInt {
    uint64_t min;  //!< @brief
    uint64_t max;  //!< @brief
};

/**
 * @brief
 */
struct genie_SegmentFilter {
    bool filterScope[genie_SegmentType::genie_SegmentType_COUNT];  //!< @brief
    genie_RangeInt mappedBasesRange;                               //!< @brief
    genie_RangeFloat mappedFractionRange;                          //!< @brief
    genie_RangeInt errorsRange;                                    //!< @brief
    genie_RangeFloat errorsFractionRange;                          //!< @brief
    genie_RangeInt substitutionsRange;                             //!< @brief
    genie_RangeFloat substitutionsFractionRange;                   //!< @brief
    genie_RangeInt insertionsRange;                                //!< @brief
    genie_RangeFloat insertionsFractionRange;                      //!< @brief
    genie_RangeInt insertionsLengthRange;                          //!< @brief
    genie_RangeFloat insertionsLengthFractionRange;                //!< @brief
    genie_RangeInt deletionsRange;                                 //!< @brief
    genie_RangeFloat deletionsFractionRange;                       //!< @brief
    genie_RangeInt deletionsLengthRange;                           //!< @brief
    genie_RangeFloat deletionsLengthFractionRange;                 //!< @brief
    genie_RangeInt splicesRange;                                   //!< @brief
    genie_RangeFloat splicesFractionRange;                         //!< @brief
    genie_RangeInt splicesLengthRange;                             //!< @brief
    int splicesDirectionAsEnd;                                     //!< @brief
    genie_RangeFloat alignmentScoreRange;                          //!< @brief
    genie_RangeInt qualityScoreRange;                              //!< @brief
};

/**
 * @brief
 */
struct genie_AdvancedFilter {
    genie_SimpleFilter filter;            //!< @brief
    uint64_t segmentFiltersCount;         //!< @brief
    genie_SegmentFilter* segmentFilters;  //!< @brief [segmentFiltersCount]
};

/**
 * @brief
 * @param outputHierarchy
 * @return
 */
genie_ReturnCode genie_getHierarchy(genie_Hierarchy** outputHierarchy);

/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param filter
 * @param outputRecords
 * @return
 */
genie_ReturnCode genie_getDataBySimpleFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                             const genie_SimpleFilter* filter, genie_Records** outputRecords);

/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param filter
 * @param outputRecords
 * @return
 */
genie_ReturnCode genie_getDataByAdvancedFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                               const genie_AdvancedFilter* filter, genie_Records** outputRecords);

/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param signature
 * @param outputRecords
 * @return
 */
genie_ReturnCode genie_getDataBySignature(uint64_t datasetGroupID, uint64_t datasetID, const char* signature,
                                          genie_Records** outputRecords);

/**
 * @brief
 * @param datasetGroupID
 * @param labelID
 * @param outputRecords
 * @return
 */
genie_ReturnCode genie_getDataByLabel(uint64_t datasetGroupID, const char* labelID, genie_Records** outputRecords);

/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param outputMetadata
 * @return
 */
genie_ReturnCode genie_getMetadataFields(uint64_t datasetGroupID, uint64_t datasetID, char*** outputMetadata);

/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param fieldName
 * @param outputMetadata
 * @return
 */
genie_ReturnCode genie_getMetadataContent(uint64_t datasetGroupID, uint64_t datasetID, const char* fieldName,
                                          char** outputMetadata);

/**
 * @brief
 * @param datasetGroupID
 * @param outputProtection
 * @return
 */
genie_ReturnCode genie_getDatasetGroupProtection(uint64_t datasetGroupID, char** outputProtection);

/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param outputProtection
 * @return
 */
genie_ReturnCode genie_getDatasetProtection(uint64_t datasetGroupID, uint64_t datasetID, char** outputProtection);

/**
 * @brief
 * @param datasetGroup_ID
 * @param datasetID
 * @param sequenceID
 * @param startPos
 * @param endPos
 * @param outputProtection
 * @return
 */
genie_ReturnCode genie_getDatasetRegionProtection(uint64_t datasetGroup_ID, uint64_t datasetID, uint64_t sequenceID,
                                                  uint64_t startPos, uint64_t endPos,
                                                  genie_RegionProtection** outputProtection);
/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param includeSequences
 * @param outputReference
 * @return
 */
genie_ReturnCode genie_getDatasetReference(uint64_t datasetGroupID, uint64_t datasetID, bool includeSequences,
                                           genie_Reference* outputReference);
/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param sequenceID
 * @param startPos
 * @param endPos
 * @param maxSegments
 * @param outputStatistics
 * @return
 */
genie_ReturnCode genie_getSimpleStatistics(uint64_t datasetGroupID, uint64_t datasetID, uint64_t sequenceID,
                                           uint64_t startPos, uint64_t endPos, uint64_t* maxSegments,
                                           genie_SimpleSegmentStatistics** outputStatistics);

/**
 * @brief
 * @param datasetGroupID
 * @param datasetID
 * @param sequenceID
 * @param startPos
 * @param endPos
 * @param maxSegments
 * @param outputStatistics
 * @return
 */
genie_ReturnCode genie_getAdvancedStatistics(uint64_t datasetGroupID, uint64_t datasetID, uint64_t sequenceID,
                                             uint64_t startPos, uint64_t endPos, uint64_t* maxSegments,
                                             genie_AdvancedSegmentStatistics** outputStatistics);

// ---------------------------------------------------------------------------------------------------------------------
}

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_C_API_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
