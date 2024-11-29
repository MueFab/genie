/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_API_H_
#define SRC_GENIE_CORE_API_H_

// -----------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/record.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::api {

/**
 * @brief
 */
class ExceptionPartiallyAuthorized final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionNotAuthorized final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionVerificationFailed final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionDecryptionFailed final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionDatasetGroupNotFound final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionDatasetNotFound final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionAccessUnitNotFound final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionReferenceNotFound final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionSequenceNotFound final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionMetadataFieldNotFound final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionMetadataInvalid final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionReferenceInvalid final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionParameterInvalid final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
class ExceptionBitstreamInvalid final : public util::RuntimeException {
 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string Msg() const override;
};

/**
 * @brief
 */
constexpr size_t kAcgtn_Size = 5;

/**
 * @brief
 */
enum class SegmentType : uint8_t {
  kSingle = 0,
  kFirst = 1,
  kSecond = 2,
  kCount = 3
};

/**
 * @brief
 */
enum class StatisticsIndex : uint8_t {
  kMinimum = 0,
  kMaximum = 1,
  kAverage = 2,
  kCount = 3
};

/**
 * @brief
 */
enum class Strand : uint8_t {
  kUnmappedUnknown = 0,
  kForward = 1,
  kReverse = 2,
  kCount = 3
};

/**
 * @brief
 */
enum class StrandStrict : uint8_t { kForward = 0, kReverse = 1, kCount = 2 };

/**
 * @brief
 */
enum class StrandPaired : uint8_t {
  kUnmappedUnmapped = 0,
  kUnmappedForward = 1,
  kUnmappedReverse = 2,
  kForwardUnmapped = 3,
  kReverseUnmapped = 4,
  kForwardForward = 5,
  kForwardReverse = 6,
  kReverseForward = 7,
  kReverseReverse = 8,
  kCount = 9
};

/**
 * @brief
 */
enum class ClipType : uint8_t { kSoft = 0, kHard = 1, kCount = 2 };

/**
 * @brief
 */
enum class ClipTypeCombination : uint8_t {
  kNone = 0,
  kSoft = 1,
  kSoftHard = 2,
  kCount = 3
};

/**
 * @brief
 */
enum class PhredBins : uint8_t {
  kBin010 = 0,
  kBin1025 = 1,
  kBin2550 = 2,
  kBin5075 = 3,
  kBin7590 = 4,
  kBin9010 = 5,
  kCount = 6
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
  std::variant<int32_t, std::string, uint8_t, int8_t, uint16_t, int16_t,
               uint32_t, HexCode, float, double>
      value;  //!< @brief
};

/**
 * @brief
 */
struct GenAux {
  std::vector<GenTag> aux_fields;  //!< @brief
};

/**
 * @brief
 */
struct GenAuxRecord {
  std::vector<GenAux> aux_set;  //!< @brief [numberOfGenAux]
};

/**
 * @brief
 */
struct Records {
  uint64_t dataset_group_id;            //!< @brief
  uint64_t dataset_id;                  //!< @brief
  std::vector<record::Record> records;  //!< @brief [recordsCount]
  std::vector<GenAuxRecord> aux_info;   //!< @brief [recordsCount]
};

/**
 * @brief
 */
struct Sequence {
  std::string seq_name;      //!< @brief
  uint64_t seq_start;        //!< @brief
  uint64_t seq_end;          //!< @brief
  std::string ref_sequence;  //!< @brief
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
  std::string sequence_name;           //!< @brief
  uint64_t start_pos;                  //!< @brief
  uint64_t end_pos;                    //!< @brief
  uint64_t class_id;                   //!< @brief
  std::vector<std::string> key_names;  //!< @brief
};

/**
 * @brief
 */
struct SimpleSegmentStatistics {
  uint64_t reads_number;  //!< @brief
  std::vector<uint64_t>
      segments_number_reads_distribution;      //!< @brief [GENIE_MAX_SEGMENTS]
  uint64_t quality_check_failed_reads_number;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      segment_length;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(Strand::kCount)>
      mapped_strand_segment_distribution;  //!< @brief
  uint64_t properly_paired_number;         //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StrandPaired::kCount)>
      mapped_strand_pair_distribution;  //!< @brief
  uint64_t max_alignments;              //!< @brief
  std::vector<uint64_t>
      multiple_alignment_segment_distribution;  //!< @brief
                                                //!< [GENIE_MAX_ALIGNMENTS
                                                //!< + 1];
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      coverage;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      weighted_coverage;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      errors_number;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      substitutions_number;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      insertions_number;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      insertions_length;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      deletions_number;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      deletions_length;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      splices_number;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      splices_length;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>
      alignment_score;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(record::ClassType::kCount)>
      class_segment_distribution;  //!< @brief
  std::array<uint64_t, static_cast<uint8_t>(ClipTypeCombination::kCount)>
      clipped_segment_distribution;    //!< @brief
  uint64_t optical_duplicates_number;  //!< @brief
  uint64_t chimeras_number;            //!< @brief
};

/**
 * @brief
 */
struct ExtendedSegmentStatistics {
  std::vector<uint64_t>
      mapped_bases_number_distribution;     //!< @brief [segmentLength[1] + 1]
  std::vector<uint64_t> coverage;           //!< @brief [endPos – startPos + 1]
  std::vector<uint64_t> weighted_coverage;  //!< @brief [endPos – startPos + 1]
  std::vector<uint64_t>
      errors_number_distribution;  //!< @brief [errorsNumber[1] + 1]
  std::vector<uint64_t>
      errors_position_distribution;  //!< @brief [segmentLength[1]]
  std::vector<uint64_t>
      substitutions_number_distribution;  //!< @brief [substitutionsNumber[1]
                                          //!< + 1]
  std::vector<std::array<std::array<uint64_t, kAcgtn_Size>, kAcgtn_Size>>
      substitutions_transition_distribution;  //!< @brief [segmentLength[1]]
  std::vector<uint64_t>
      substitutions_position_distribution;  //!< @brief segmentLength[1]
  std::vector<uint64_t>
      insertions_number_distribution;  //!< @brief [insertionsNumber[1] + 1]
  std::vector<uint64_t>
      insertions_length_distribution;  //!< @brief [insertionsLength[1] + 1]
  std::vector<uint64_t>
      insertions_position_distribution;  //!< @brief [segmentLength[1]]
  std::vector<uint64_t>
      deletions_number_distribution;  //!< @brief [deletionsNumber[1] + 1]
  std::vector<uint64_t>
      deletions_length_distribution;  //!< @brief[deletionsLength[1] + 1]
  std::vector<uint64_t>
      deletions_position_distribution;  //!< @brief [segmentLength[1]]
  std::vector<std::array<uint64_t, static_cast<uint8_t>(Strand::kCount)>>
      splices_number_distribution;  //!< @brief [splicesNumber[1] + 1],
  std::vector<std::array<uint64_t, static_cast<uint8_t>(Strand::kCount)>>
      splices_length_distribution;  //!< @brief [splicesLength[1] + 1],
  std::vector<std::array<uint64_t, static_cast<uint8_t>(Strand::kCount)>>
      splices_position_distribution;  //!< @brief [segmentLength[1]],
  std::vector<uint64_t>
      alignment_score_value_distribution;  //!< @brief [alignmentScore[1] + 1]
  std::vector<
      std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>>
      alignment_score_segment_length_distribution;  //!< @brief
                                                    //!< [segmentLength[1]]
  std::vector<std::array<uint64_t, kAcgtn_Size>>
      bases_position_distribution;  //!< @brief [segmentLength[1]]
  std::array<uint64_t, 101>
      gc_content_value_distribution;  //!< @brief GC content in percent 0-100
  std::vector<uint64_t>
      quality_score_distribution;  //!< @brief [maxQualityScore + 1]
  std::vector<
      std::array<uint64_t, static_cast<uint8_t>(StatisticsIndex::kCount)>>
      quality_score_position_distribution;  //!< @brief [segmentLength[1]]
  std::vector<std::array<uint64_t, static_cast<uint8_t>(PhredBins::kCount)>>
      quality_score_position_percentiles_distribution;
  //!< @brief
  //!< [segmentLength[1]],
};

/**
 * @brief
 */
struct AdvancedSegmentStatistics {
  std::array<SimpleSegmentStatistics,
             static_cast<uint8_t>(StrandStrict::kCount)>
      simple_statistics;  //!< @brief
  std::array<ExtendedSegmentStatistics,
             static_cast<uint8_t>(StrandStrict::kCount)>
      extended_statistics;  //!< @brief
};

/**
 * @brief
 */
struct SimpleFilter {
  std::vector<std::string> group_names;  //!< @brief
  std::array<bool, static_cast<uint8_t>(record::ClassType::kCount)>
      class_id;          //!< @brief
  uint64_t sequence_id;  //!< @brief
  uint64_t start_pos;    //!< @brief
  uint64_t end_pos;      //!< @brief
  std::array<bool, static_cast<uint8_t>(Strand::kCount)>
      single_ends_strand;  //!< @brief
  std::array<bool, static_cast<uint8_t>(StrandPaired::kCount)>
      paired_ends_strand;  //!< @brief
  std::array<bool, static_cast<uint8_t>(ClipType::kCount)>
      include_clipped_reads;          //!< @brief
  bool include_multiple_alignments;   //!< @brief
  bool include_optical_duplicates;    //!< @brief
  bool include_quality_check_failed;  //!< @brief
  bool include_aux_records;           //!< @brief
  bool include_read_names;            //!< @brief
  bool include_quality_values;        //!< @brief
  bool mismatches_include_ns;         //!< @brief
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

  std::array<bool, static_cast<uint8_t>(SegmentType::kCount)>
      filter_scope;                               //!< @brief
  Range<uint64_t> mapped_bases_range;             //!< @brief
  Range<float> mapped_fraction_range;             //!< @brief
  Range<uint64_t> errors_range;                   //!< @brief
  Range<float> errors_fraction_range;             //!< @brief
  Range<uint64_t> substitutions_range;            //!< @brief
  Range<float> substitutions_fraction_range;      //!< @brief
  Range<uint64_t> insertions_range;               //!< @brief
  Range<float> insertions_fraction_range;         //!< @brief
  Range<uint64_t> insertions_length_range;        //!< @brief
  Range<float> insertions_length_fraction_range;  //!< @brief
  Range<uint64_t> deletions_range;                //!< @brief
  Range<float> deletions_fraction_range;          //!< @brief
  Range<uint64_t> deletions_length_range;         //!< @brief
  Range<float> deletions_length_fraction_range;   //!< @brief
  Range<uint64_t> splices_range;                  //!< @brief
  Range<float> splices_fraction_range;            //!< @brief
  Range<uint64_t> splices_length_range;           //!< @brief
  int splices_direction_as_end;                   //!< @brief
  Range<float> alignment_score_range;             //!< @brief
  Range<uint64_t> quality_score_range;            //!< @brief
};

/**
 * @brief
 */
struct AdvancedFilter {
  SimpleFilter filter;                         //!< @brief
  std::vector<SegmentFilter> segment_filters;  //!< @brief
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
  static Hierarchy GetHierarchy();

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param filter
   * @return
   */
  static std::vector<Records> GetDataBySimpleFilter(uint64_t dataset_group_id,
                                                    uint64_t dataset_id,
                                                    const SimpleFilter& filter);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param filter
   * @return
   */
  static std::vector<Records> GetDataByAdvancedFilter(
      uint64_t dataset_group_id, uint64_t dataset_id,
      const AdvancedFilter& filter);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param signature
   * @return
   */
  static std::vector<Records> GetDataBySignature(uint64_t dataset_group_id,
                                                 uint64_t dataset_id,
                                                 const char* signature);

  /**
   * @brief
   * @param dataset_group_id
   * @param label_id
   * @return
   */
  static std::vector<Records> GetDataByLabel(uint64_t dataset_group_id,
                                             const std::string& label_id);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @return
   */
  static std::vector<std::string> GetMetadataFields(uint64_t dataset_group_id,
                                                    uint64_t dataset_id);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param field_name
   * @return
   */
  static std::string GetMetadataContent(uint64_t dataset_group_id,
                                        uint64_t dataset_id,
                                        const std::string& field_name);

  /**
   * @brief
   * @param dataset_group_id
   * @return
   */
  static std::string GetDatasetGroupProtection(uint64_t dataset_group_id);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @return
   */
  static std::string GetDatasetProtection(uint64_t dataset_group_id,
                                          uint64_t dataset_id);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param sequence_id
   * @param start_pos
   * @param end_pos
   * @return
   */
  static std::vector<RegionProtection> GetDatasetRegionProtection(
      uint64_t dataset_group_id, uint64_t dataset_id, uint64_t sequence_id,
      uint64_t start_pos, uint64_t end_pos);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param include_sequences
   * @return
   */
  static OutReference GetDatasetReference(uint64_t dataset_group_id,
                                          uint64_t dataset_id,
                                          bool include_sequences);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param sequence_id
   * @param start_pos
   * @param end_pos
   * @return
   */
  static std::vector<SimpleSegmentStatistics> GetSimpleStatistics(
      uint64_t dataset_group_id, uint64_t dataset_id, uint64_t sequence_id,
      uint64_t start_pos, uint64_t end_pos);

  /**
   * @brief
   * @param dataset_group_id
   * @param dataset_id
   * @param sequence_id
   * @param start_pos
   * @param end_pos
   * @return
   */
  static std::vector<AdvancedSegmentStatistics> GetAdvancedStatistics(
      uint64_t dataset_group_id, uint64_t dataset_id, uint64_t sequence_id,
      uint64_t start_pos, uint64_t end_pos);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::api

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_API_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
