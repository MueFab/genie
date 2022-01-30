
#ifndef GENIE_API_H
#define GENIE_API_H

#include <cstdint>
#include <string>
#include <vector>

#include "boost/variant/variant.hpp"
#include "genie/core/constants.h"
#include "genie/core/record/record.h"
#include "genie/util/runtime-exception.h"

namespace genie {
namespace core {
namespace api {

class ExceptionPartiallyAuthorized : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Only partially authorized. " + genie::util::RuntimeException::msg(); }
};

class ExceptionNotAuthorized : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Not authorized. " + genie::util::RuntimeException::msg(); }
};
class ExceptionVerificationFailed : public genie::util::RuntimeException {
 public:
    std::string msg() const override {
        return "Signature verification failed. " + genie::util::RuntimeException::msg();
    }
};
class ExceptionDecryptionFailed : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Decryption failed. " + genie::util::RuntimeException::msg(); }
};
class ExceptionDatasetGroupNotFound : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Dataset group not found. " + genie::util::RuntimeException::msg(); }
};
class ExceptionDatasetNotFound : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Dataset not found. " + genie::util::RuntimeException::msg(); }
};
class ExceptionAccessUnitNotFound : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Access unit not found. " + genie::util::RuntimeException::msg(); }
};
class ExceptionReferenceNotFound : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Reference not found. " + genie::util::RuntimeException::msg(); }
};
class ExceptionSequenceNotFound : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Sequence not found. " + genie::util::RuntimeException::msg(); }
};
class ExceptionMetadataFieldNotFound : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Metadata field not found. " + genie::util::RuntimeException::msg(); }
};
class ExceptionMetadataInvalid : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Metadata invalid. " + genie::util::RuntimeException::msg(); }
};
class ExceptionReferenceInvalid : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Reference invalid. " + genie::util::RuntimeException::msg(); }
};
class ExceptionParameterInvalid : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Parameter invalid. " + genie::util::RuntimeException::msg(); }
};
class ExceptionBitstreamInvalid : public genie::util::RuntimeException {
 public:
    std::string msg() const override { return "Bitstream invalid." + genie::util::RuntimeException::msg(); }
};

constexpr size_t ACGTN_SIZE = 5;

enum class SegmentType : uint8_t { SINGLE = 0, FIRST = 1, SECOND = 2, COUNT = 3 };

enum class StatisticsIndex : uint8_t { MINIMUM = 0, MAXIMUM = 1, AVERAGE = 2, COUNT = 3 };

enum class Strand : uint8_t { UNMAPPED_UNKNOWN = 0, FORWARD = 1, REVERSE = 2, COUNT = 3 };

enum class StrandStrict : uint8_t { FORWARD = 0, REVERSE = 1, COUNT = 2 };

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

enum class ClipType : uint8_t { SOFT = 0, HARD = 1, COUNT = 2 };

enum class ClipTypeCombination : uint8_t { NONE = 0, SOFT = 1, SOFT_HARD = 2, COUNT = 3 };

enum class PhredBins : uint8_t {
    BIN_0_10 = 0,
    BIN_10_25 = 1,
    BIN_25_50 = 2,
    BIN_50_75 = 3,
    BIN_75_90 = 4,
    BIN_90_10 = 5,
    COUNT = 6
};

struct Hierarchy {
    struct DatasetGroup {
        uint64_t id;
        std::vector<uint64_t> dataset_ids;
    };
    std::vector<DatasetGroup> groups;
};

struct HexCode {
    std::string value;
};

struct GenTag {
    char key[2];
    boost::variant<int32_t, std::string, uint8_t, int8_t, uint16_t, int16_t, uint32_t, HexCode, float, double> value;
};

struct GenAux {
    std::vector<GenTag> auxFields;
};

struct GenAuxRecord {
    std::vector<GenAux> auxSet;  // [numberOfGenAux]
};

struct Records {
    uint64_t datasetGroupID;
    uint64_t datasetID;
    std::vector<genie::core::record::Record> records;  // [recordsCount]
    std::vector<GenAuxRecord> auxInfo;                 // [recordsCount]
};

struct Sequence {
    std::string seqName;
    uint64_t seqStart;
    uint64_t seqEnd;
    std::string refSequence;
};

struct OutReference {
    std::vector<Sequence> sequences;
};

struct RegionProtection {
    std::string sequenceName;
    uint64_t startPos;
    uint64_t endPos;
    uint64_t classID;
    std::vector<std::string> keyNames;
};

struct SimpleSegmentStatistics {
    uint64_t readsNumber;
    std::vector<uint64_t> segmentsNumberReadsDistribution;  // [GENIE_MAX_SEGMENTS]
    uint64_t qualityCheckFailedReadsNumber;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> segmentLength;
    std::array<uint64_t, uint8_t(Strand::COUNT)> mappedStrandSegmentDistribution;
    uint64_t properlyPairedNumber;
    std::array<uint64_t, uint8_t(StrandPaired::COUNT)> mappedStrandPairDistribution;
    uint64_t maxAlignments;
    std::vector<uint64_t> multipleAlignmentSegmentDistribution;  // [GENIE_MAX_ALIGNMENTS + 1];
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> coverage;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> weightedCoverage;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> errorsNumber;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> substitutionsNumber;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> insertionsNumber;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> insertionsLength;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> deletionsNumber;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> deletionsLength;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> splicesNumber;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> splicesLength;
    std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)> alignmentScore;
    std::array<uint64_t, uint8_t(genie::core::record::ClassType::COUNT)> classSegmentDistribution;
    std::array<uint64_t, uint8_t(ClipTypeCombination::COUNT)> clippedSegmentDistribution;
    uint64_t opticalDuplicatesNumber;
    uint64_t chimerasNumber;
};

struct ExtendedSegmentStatistics {
    std::vector<uint64_t> mappedBasesNumberDistribution;    // [segmentLength[1] + 1]
    std::vector<uint64_t> coverage;                         // [endPos – startPos + 1]
    std::vector<uint64_t> weightedCoverage;                 // [endPos – startPos + 1]
    std::vector<uint64_t> errorsNumberDistribution;         // [errorsNumber[1] + 1]
    std::vector<uint64_t> errorsPositionDistribution;       // [segmentLength[1]]
    std::vector<uint64_t> substitutionsNumberDistribution;  // [substitutionsNumber[1] + 1]
    std::vector<std::array<std::array<uint64_t, ACGTN_SIZE>, ACGTN_SIZE>>
        substitutionsTransitionDistribution;                                                // [segmentLength[1]]
    std::vector<uint64_t> substitutionsPositionDistribution;                                // segmentLength[1]
    std::vector<uint64_t> insertionsNumberDistribution;                                     // [insertionsNumber[1] + 1]
    std::vector<uint64_t> insertionsLengthDistribution;                                     // [insertionsLength[1] + 1]
    std::vector<uint64_t> insertionsPositionDistribution;                                   // [segmentLength[1]]
    std::vector<uint64_t> deletionsNumberDistribution;                                      // [deletionsNumber[1] + 1]
    std::vector<uint64_t> deletionsLengthDistribution;                                      // [deletionsLength[1] + 1]
    std::vector<uint64_t> deletionsPositionDistribution;                                    // [segmentLength[1]]
    std::vector<std::array<uint64_t, uint8_t(Strand::COUNT)>> splicesNumberDistribution;    // [splicesNumber[1] + 1],
    std::vector<std::array<uint64_t, uint8_t(Strand::COUNT)>> splicesLengthDistribution;    // [splicesLength[1] + 1],
    std::vector<std::array<uint64_t, uint8_t(Strand::COUNT)>> splicesPositionDistribution;  // [segmentLength[1]],
    std::vector<uint64_t> alignmentScoreValueDistribution;                                  // [alignmentScore[1] + 1]
    std::vector<std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)>>
        alignmentScoreSegmentLengthDistribution;                              // [segmentLength[1]]
    std::vector<std::array<uint64_t, ACGTN_SIZE>> basesPositionDistribution;  // [segmentLength[1]]
    std::array<uint64_t, 101> gcContentValueDistribution;                     // GC content in percent 0-100
    std::vector<uint64_t> qualityScoreDistribution;                           // [maxQualityScore + 1]
    std::vector<std::array<uint64_t, uint8_t(StatisticsIndex::COUNT)>>
        qualityScorePositionDistribution;  // [segmentLength[1]]
    std::vector<std::array<uint64_t, uint8_t(PhredBins::COUNT)>>
        qualityScorePositionPercentilesDistribution;  // [segmentLength[1]],
};

struct AdvancedSegmentStatistics {
    std::array<SimpleSegmentStatistics, uint8_t(StrandStrict::COUNT)> simpleStatistics;
    std::array<ExtendedSegmentStatistics, uint8_t(StrandStrict::COUNT)> extendedStatistics;
};

struct SimpleFilter {
    std::vector<std::string> groupNames;
    std::array<bool, uint8_t(core::record::ClassType::COUNT)> classID;
    uint64_t sequenceID;
    uint64_t startPos;
    uint64_t endPos;
    std::array<bool, uint8_t(Strand::COUNT)> singleEndsStrand;
    std::array<bool, uint8_t(StrandPaired::COUNT)> pairedEndsStrand;
    std::array<bool, uint8_t(ClipType::COUNT)> includeClippedReads;
    bool includeMultipleAlignments;
    bool includeOpticalDuplicates;
    bool includeQualityCheckFailed;
    bool includeAuxRecords;
    bool includeReadNames;
    bool includeQualityValues;
    bool mismatchesIncludeNs;
};

struct SegmentFilter {
    template <typename T>
    struct Range {
        T min;
        T max;
    };

    std::array<bool, uint8_t(SegmentType::COUNT)> filterScope;
    Range<uint64_t> mappedBasesRange;
    Range<float> mappedFractionRange;
    Range<uint64_t> errorsRange;
    Range<float> errorsFractionRange;
    Range<uint64_t> substitutionsRange;
    Range<float> substitutionsFractionRange;
    Range<uint64_t> insertionsRange;
    Range<float> insertionsFractionRange;
    Range<uint64_t> insertionsLengthRange;
    Range<float> insertionsLengthFractionRange;
    Range<uint64_t> deletionsRange;
    Range<float> deletionsFractionRange;
    Range<uint64_t> deletionsLengthRange;
    Range<float> deletionsLengthFractionRange;
    Range<uint64_t> splicesRange;
    Range<float> splicesFractionRange;
    Range<uint64_t> splicesLengthRange;
    int splicesDirectionAsEnd;
    Range<float> alignmentScoreRange;
    Range<uint64_t> qualityScoreRange;
};

struct AdvancedFilter {
    SimpleFilter filter;
    std::vector<SegmentFilter> segmentFilters;
};

class GenieState {
 private:
 public:
    Hierarchy getHierarchy() {
        UTILS_DIE("Not implemented");
        return Hierarchy{};
    }

    std::vector<Records> getDataBySimpleFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                                  const SimpleFilter& filter) {
        (void) datasetGroupID;
        (void) datasetID;
        (void) filter;
        UTILS_DIE("Not implemented");
        return std::vector<Records>();
    }

    std::vector<Records> getDataByAdvancedFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                                          const AdvancedFilter& filter) {
        (void) datasetGroupID;
        (void) datasetID;
        (void) filter;
        UTILS_DIE("Not implemented");
        return std::vector<Records>();
    }

    std::vector<Records> getDataBySignature(uint64_t datasetGroupID, uint64_t datasetID, const char* signature) {
        (void) datasetGroupID;
        (void) datasetID;
        (void) signature;
        UTILS_DIE("Not implemented");
        return std::vector<Records>();
    }

    std::vector<Records> getDataByLabel(uint64_t datasetGroupID, const std::string& labelID) {
        (void) datasetGroupID;
        (void) labelID;
        UTILS_DIE("Not implemented");
        return std::vector<Records>();
    }

    std::vector<std::string> getMetadataFields(uint64_t datasetGroupID, uint64_t datasetID) {
        (void) datasetGroupID;
        (void) datasetID;
        UTILS_DIE("Not implemented");
        return std::vector<std::string>();
    }

    std::string getMetadataContent(uint64_t datasetGroupID, uint64_t datasetID, const std::string& fieldName) {
        (void) datasetGroupID;
        (void) datasetID;
        (void) fieldName;
        UTILS_DIE("Not implemented");
        return std::string();
    }

    std::string getDatasetGroupProtection(uint64_t datasetGroupID) {
        (void) datasetGroupID;
        UTILS_DIE("Not implemented");
        return std::string();
    }

    std::string getDatasetProtection(uint64_t datasetGroupID, uint64_t datasetID) {
        (void) datasetGroupID;
        (void) datasetID;
        UTILS_DIE("Not implemented");
        return std::string();
    }

    std::vector<RegionProtection> getDatasetRegionProtection(uint64_t datasetGroupID, uint64_t datasetID, uint64_t sequenceID,
                                                      uint64_t startPos, uint64_t endPos) {
        (void) datasetGroupID;
        (void) datasetID;
        (void) sequenceID;
        (void) startPos;
        (void) endPos;
        UTILS_DIE("Not implemented");
        return std::vector<RegionProtection>();
    }

    OutReference getDatasetReference(uint64_t datasetGroupID, uint64_t datasetID, bool includeSequences){
        (void) datasetGroupID;
        (void) datasetID;
        (void) includeSequences;
        UTILS_DIE("Not implemented");
        return OutReference();
    }

    std::vector<SimpleSegmentStatistics> getSimpleStatistics(uint64_t datasetGroupID, uint64_t datasetID, uint64_t sequenceID,
                                               uint64_t startPos, uint64_t endPos) {
        (void) datasetGroupID;
        (void) datasetID;
        (void) sequenceID;
        (void) startPos;
        (void) endPos;
        UTILS_DIE("Not implemented");
        return std::vector<SimpleSegmentStatistics>();
    }

    std::vector<AdvancedSegmentStatistics> getAdvancedStatistics(uint64_t datasetGroupID, uint64_t datasetID, uint64_t sequenceID,
                                                 uint64_t startPos, uint64_t endPos) {
        (void) datasetGroupID;
        (void) datasetID;
        (void) sequenceID;
        (void) startPos;
        (void) endPos;
        UTILS_DIE("Not implemented");
        return std::vector<AdvancedSegmentStatistics>();
    }
};

}  // namespace api
}  // namespace core
}  // namespace genie

#endif  // GENIE_API_H
