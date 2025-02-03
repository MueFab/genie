/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_API_H_
#define SRC_GENIE_CORE_API_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "boost/variant/variant.hpp"
#include "genie/core/constants.h"
#include "genie/core/record/alignment/record.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace api {

/**
 * @brief
 */
class ExceptionPartiallyAuthorized : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionNotAuthorized : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionVerificationFailed : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionDecryptionFailed : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionDatasetGroupNotFound : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionDatasetNotFound : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionAccessUnitNotFound : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionReferenceNotFound : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionSequenceNotFound : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionMetadataFieldNotFound : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionMetadataInvalid : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionReferenceInvalid : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionParameterInvalid : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
class ExceptionBitstreamInvalid : public genie::util::RuntimeException {
 public:
    /**
     * @brief
     * @return
     */
    std::string msg() const override;
};

/**
 * @brief
 */
constexpr size_t ACGTN_SIZE = 5;

/**
 * @brief
 */
enum class SegmentType : uint8_t { SINGLE = 0, FIRST = 1, SECOND = 2, COUNT = 3 };

/**
 * @brief
 */
enum class StatisticsIndex : uint8_t { MINIMUM = 0, MAXIMUM = 1, AVERAGE = 2, COUNT = 3 };

/**
 * @brief
 */
enum class Strand : uint8_t { UNMAPPED_UNKNOWN = 0, FORWARD = 1, REVERSE = 2, COUNT = 3 };

/**
 * @brief
 */
enum class StrandStrict : uint8_t { FORWARD = 0, REVERSE = 1, COUNT = 2 };

/**
 * @brief
 */
enum class StrandPaired : uint8_t {
    UNMAPPED_UNMAPPED = 0,
    UNMAPPED_FORWARD = 1,
    UNMAPPED_REVERSE = 2,
    FORWARD_UNMAPPED = 3,
    REVERSE_UNMAPPED = 4,
    FORWARD_FORWARD = 5,
    FORWARD_REVERSE = 6,
    REVERSE_FORWARD = 7,
    REVERSE_REVERSE = 8,
    COUNT = 9
};

/**
 * @brief
 */
enum class ClipType : uint8_t { SOFT = 0, HARD = 1, COUNT = 2 };

/**
 * @brief
 */
enum class ClipTypeCombination : uint8_t { NONE = 0, SOFT = 1, SOFT_HARD = 2, COUNT = 3 };

/**
 * @brief
 */
enum class PhredBins : uint8_t {
    BIN_0_10 = 0,
    BIN_10_25 = 1,
    BIN_25_50 = 2,
    BIN_50_75 = 3,
    BIN_75_90 = 4,
    BIN_90_10 = 5,
    COUNT = 6
};

/**
 * @brief
 */
struct Hierarchy {
    /**
     * @brief
     */
    struct DatasetGroup {
        uint64_t id;                        //!< @brief
        std::vector<uint64_t> dataset_ids;  //!< @brief
    };
    std::vector<DatasetGroup> groups;  //!< @brief
};

/**
 * @brief
 */
struct HexCode {
    std::string value;  //!< @brief
};

/**
 * @brief
 */
struct GenTag {
    char key[2];  //!< @brief
    boost::variant<int32_t, std::string, uint8_t, int8_t, uint16_t, int16_t, uint32_t, HexCode, float, double>
        value;  //!< @brief
};

/**
 * @brief
 */
struct GenAux {
    std::vector<GenTag> auxFields;  //!< @brief
};

/**
 * @brief
 */
struct GenAuxRecord {
    std::vector<GenAux> auxSet;  //!< @brief [numberOfGenAux]
};

/**
 * @brief
 */
struct Records {
    uint64_t datasetGroupID;                           //!< @brief
    uint64_t datasetID;                                //!< @brief
    std::vector<genie::core::record::Record> records;  //!< @brief [recordsCount]
    std::vector<GenAuxRecord> auxInfo;                 //!< @brief [recordsCount]
};

/**
 * @brief
 */
struct Sequence {
    std::string seqName;      //!< @brief
    uint64_t seqStart;        //!< @brief
    uint64_t seqEnd;          //!< @brief
    std::string refSequence;  //!< @brief
};

/**
 * @brief
 */
struct OutReference {
    std::vector<Sequence> sequences;  //!< @brief
};

/**
 * @brief
 */
struct RegionProtection {
    std::string sequenceName;           //!< @brief
    uint64_t startPos;                  //!< @brief
    uint64_t endPos;                    //!< @brief
    uint64_t classID;                   //!< @brief
    std::vector<std::string> keyNames;  //!< @brief
};

/**
 * @brief
 */
struct SimpleSegmentStatistics {
    uint64_t readsNumber;                                                             //!< @brief
    std::vector<uint64_t> segmentsNumberReadsDistribution;                            //!< @brief [GENIE_MAX_SEGMENTS]
    uint64_t qualityCheckFailedReadsNumber;                                           //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> segmentLength;              //!< @brief
    std::array<uint64_t, uint8_t(Strand::COUNT)> mappedStrandSegmentDistribution;     //!< @brief
    uint64_t properlyPairedNumber;                                                    //!< @brief
    std::array<uint64_t, uint8_t(StrandPaired::COUNT)> mappedStrandPairDistribution;  //!< @brief
    uint64_t maxAlignments;                                                           //!< @brief
    std::vector<uint64_t> multipleAlignmentSegmentDistribution;                 //!< @brief [GENIE_MAX_ALIGNMENTS + 1];
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> coverage;             //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> weightedCoverage;     //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> errorsNumber;         //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> substitutionsNumber;  //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> insertionsNumber;     //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> insertionsLength;     //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> deletionsNumber;      //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> deletionsLength;      //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> splicesNumber;        //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> splicesLength;        //!< @brief
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> alignmentScore;       //!< @brief
    std::array<uint64_t, uint8_t(genie::core::record::ClassType::COUNT)> classSegmentDistribution;  //!< @brief
    std::array<uint64_t, uint8_t(ClipTypeCombination::COUNT)> clippedSegmentDistribution;           //!< @brief
    uint64_t opticalDuplicatesNumber;                                                               //!< @brief
    uint64_t chimerasNumber;                                                                        //!< @brief
};

/**
 * @brief
 */
struct ExtendedSegmentStatistics {
    std::vector<uint64_t> mappedBasesNumberDistribution;    //!< @brief [segmentLength[1] + 1]
    std::vector<uint64_t> coverage;                         //!< @brief [endPos – startPos + 1]
    std::vector<uint64_t> weightedCoverage;                 //!< @brief [endPos – startPos + 1]
    std::vector<uint64_t> errorsNumberDistribution;         //!< @brief [errorsNumber[1] + 1]
    std::vector<uint64_t> errorsPositionDistribution;       //!< @brief [segmentLength[1]]
    std::vector<uint64_t> substitutionsNumberDistribution;  //!< @brief [substitutionsNumber[1] + 1]
    std::vector<std::array<std::array<uint64_t, ACGTN_SIZE>, ACGTN_SIZE>>
        substitutionsTransitionDistribution;                  //!< @brief [segmentLength[1]]
    std::vector<uint64_t> substitutionsPositionDistribution;  //!< @brief segmentLength[1]
    std::vector<uint64_t> insertionsNumberDistribution;       //!< @brief [insertionsNumber[1] + 1]
    std::vector<uint64_t> insertionsLengthDistribution;       //!< @brief [insertionsLength[1] + 1]
    std::vector<uint64_t> insertionsPositionDistribution;     //!< @brief [segmentLength[1]]
    std::vector<uint64_t> deletionsNumberDistribution;        //!< @brief [deletionsNumber[1] + 1]
    std::vector<uint64_t> deletionsLengthDistribution;        //!< @brief[deletionsLength[1] + 1]
    std::vector<uint64_t> deletionsPositionDistribution;      //!< @brief [segmentLength[1]]
    std::vector<std::array<uint64_t, uint8_t(Strand::COUNT)>>
        splicesNumberDistribution;  //!< @brief [splicesNumber[1] + 1],
    std::vector<std::array<uint64_t, uint8_t(Strand::COUNT)>>
        splicesLengthDistribution;  //!< @brief [splicesLength[1] + 1],
    std::vector<std::array<uint64_t, uint8_t(Strand::COUNT)>>
        splicesPositionDistribution;                        //!< @brief [segmentLength[1]],
    std::vector<uint64_t> alignmentScoreValueDistribution;  //!< @brief [alignmentScore[1] + 1]
    std::vector<std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)>>
        alignmentScoreSegmentLengthDistribution;                              //!< @brief [segmentLength[1]]
    std::vector<std::array<uint64_t, ACGTN_SIZE>> basesPositionDistribution;  //!< @brief [segmentLength[1]]
    std::array<uint64_t, 101> gcContentValueDistribution;                     //!< @brief GC content in percent 0-100
    std::vector<uint64_t> qualityScoreDistribution;                           //!< @brief [maxQualityScore + 1]
    std::vector<std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)>>
        qualityScorePositionDistribution;  //!< @brief [segmentLength[1]]
    std::vector<std::array<uint64_t, uint8_t(PhredBins::COUNT)>>
        qualityScorePositionPercentilesDistribution;  //!< @brief [segmentLength[1]],
};

/**
 * @brief
 */
struct AdvancedSegmentStatistics {
    std::array<SimpleSegmentStatistics, uint8_t(StrandStrict::COUNT)> simpleStatistics;      //!< @brief
    std::array<ExtendedSegmentStatistics, uint8_t(StrandStrict::COUNT)> extendedStatistics;  //!< @brief
};

/**
 * @brief
 */
struct SimpleFilter {
    std::vector<std::string> groupNames;                                //!< @brief
    std::array<bool, uint8_t(core::record::ClassType::COUNT)> classID;  //!< @brief
    uint64_t sequenceID;                                                //!< @brief
    uint64_t startPos;                                                  //!< @brief
    uint64_t endPos;                                                    //!< @brief
    std::array<bool, uint8_t(Strand::COUNT)> singleEndsStrand;          //!< @brief
    std::array<bool, uint8_t(StrandPaired::COUNT)> pairedEndsStrand;    //!< @brief
    std::array<bool, uint8_t(ClipType::COUNT)> includeClippedReads;     //!< @brief
    bool includeMultipleAlignments;                                     //!< @brief
    bool includeOpticalDuplicates;                                      //!< @brief
    bool includeQualityCheckFailed;                                     //!< @brief
    bool includeAuxRecords;                                             //!< @brief
    bool includeReadNames;                                              //!< @brief
    bool includeQualityValues;                                          //!< @brief
    bool mismatchesIncludeNs;                                           //!< @brief
};

/**
 * @brief
 */
struct SegmentFilter {
    /**
     * @brief
     * @tparam T
     */
    template <typename T>
    struct Range {
        T min;  //!< @brief
        T max;  //!< @brief
    };

    std::array<bool, uint8_t(SegmentType::COUNT)> filterScope;  //!< @brief
    Range<uint64_t> mappedBasesRange;                           //!< @brief
    Range<float> mappedFractionRange;                           //!< @brief
    Range<uint64_t> errorsRange;                                //!< @brief
    Range<float> errorsFractionRange;                           //!< @brief
    Range<uint64_t> substitutionsRange;                         //!< @brief
    Range<float> substitutionsFractionRange;                    //!< @brief
    Range<uint64_t> insertionsRange;                            //!< @brief
    Range<float> insertionsFractionRange;                       //!< @brief
    Range<uint64_t> insertionsLengthRange;                      //!< @brief
    Range<float> insertionsLengthFractionRange;                 //!< @brief
    Range<uint64_t> deletionsRange;                             //!< @brief
    Range<float> deletionsFractionRange;                        //!< @brief
    Range<uint64_t> deletionsLengthRange;                       //!< @brief
    Range<float> deletionsLengthFractionRange;                  //!< @brief
    Range<uint64_t> splicesRange;                               //!< @brief
    Range<float> splicesFractionRange;                          //!< @brief
    Range<uint64_t> splicesLengthRange;                         //!< @brief
    int splicesDirectionAsEnd;                                  //!< @brief
    Range<float> alignmentScoreRange;                           //!< @brief
    Range<uint64_t> qualityScoreRange;                          //!< @brief
};

/**
 * @brief
 */
struct AdvancedFilter {
    SimpleFilter filter;                        //!< @brief
    std::vector<SegmentFilter> segmentFilters;  //!< @brief
};

/**
 * @brief
 */
class GenieState {
 public:
    /**
     * @brief
     * @return
     */
    Hierarchy getHierarchy();

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param filter
     * @return
     */
    std::vector<Records> getDataBySimpleFilter(uint64_t datasetGroupID, uint64_t datasetID, const SimpleFilter& filter);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param filter
     * @return
     */
    std::vector<Records> getDataByAdvancedFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                                 const AdvancedFilter& filter);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param signature
     * @return
     */
    std::vector<Records> getDataBySignature(uint64_t datasetGroupID, uint64_t datasetID, const char* signature);

    /**
     * @brief
     * @param datasetGroupID
     * @param labelID
     * @return
     */
    std::vector<Records> getDataByLabel(uint64_t datasetGroupID, const std::string& labelID);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @return
     */
    std::vector<std::string> getMetadataFields(uint64_t datasetGroupID, uint64_t datasetID);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param fieldName
     * @return
     */
    std::string getMetadataContent(uint64_t datasetGroupID, uint64_t datasetID, const std::string& fieldName);

    /**
     * @brief
     * @param datasetGroupID
     * @return
     */
    std::string getDatasetGroupProtection(uint64_t datasetGroupID);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @return
     */
    std::string getDatasetProtection(uint64_t datasetGroupID, uint64_t datasetID);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param sequenceID
     * @param startPos
     * @param endPos
     * @return
     */
    std::vector<RegionProtection> getDatasetRegionProtection(uint64_t datasetGroupID, uint64_t datasetID,
                                                             uint64_t sequenceID, uint64_t startPos, uint64_t endPos);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param includeSequences
     * @return
     */
    OutReference getDatasetReference(uint64_t datasetGroupID, uint64_t datasetID, bool includeSequences);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param sequenceID
     * @param startPos
     * @param endPos
     * @return
     */
    std::vector<SimpleSegmentStatistics> getSimpleStatistics(uint64_t datasetGroupID, uint64_t datasetID,
                                                             uint64_t sequenceID, uint64_t startPos, uint64_t endPos);

    /**
     * @brief
     * @param datasetGroupID
     * @param datasetID
     * @param sequenceID
     * @param startPos
     * @param endPos
     * @return
     */
    std::vector<AdvancedSegmentStatistics> getAdvancedStatistics(uint64_t datasetGroupID, uint64_t datasetID,
                                                                 uint64_t sequenceID, uint64_t startPos,
                                                                 uint64_t endPos);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace api
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_API_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
