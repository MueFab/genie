/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_C_API_H_
#define SRC_GENIE_CORE_C_API_H_

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

#include <stdint.h>

/**
 * @brief
 */
enum GenieReturnCode : uint8_t {
  kGenieReturnCodeGSuccess = 0,
  kGenieReturnCodeGPartiallyAuthorized = 1,
  kGenieReturnCodeGNotAuthorized = 2,
  kGenieReturnCodeGVerificationFailed = 3,
  kGenieReturnCodeGDecryptionFailed = 4,
  kGenieReturnCodeGDatasetgroupNotfound = 5,
  kGenieReturnCodeGDatasetNotfound = 6,
  kGenieReturnCodeGAccessunitsNotfound = 7,
  kGenieReturnCodeGReferenceNotfound = 8,
  kGenieReturnCodeGSequenceNotfound = 9,
  kGenieReturnCodeGMetadataFieldNotfound = 10,
  kGenieReturnCodeGInvalidMetadata = 11,
  kGenieReturnCodeGInvalidReference = 12,
  kGenieReturnCodeGInvalidParameter = 13,
  kGenieReturnCodeGInvalidBitstream = 14,
  kGenieReturnCodeGUnlistedError = 15
};

/**
 * @brief
 * @param rc
 * @return
 */
const char* GenieStrerror(GenieReturnCode rc);

/**
 * @brief
 */
enum GenieSegmentType : uint8_t {
  kGenieSegmentTypeSingle = 0,
  kGenieSegmentTypeFirst = 1,
  kGenieSegmentTypeSecond = 2,
  kGenieSegmentTypeCount = 3
};

/**
 * @brief
 */
enum GenieStatisticsIndex : uint8_t {
  kGenieStatisticsIndexMinimum = 0,
  kGenieStatisticsIndexMaximum = 1,
  kGenieStatisticsIndexAverage = 2,
  kGenieStatisticsIndexCount = 3
};

/**
 * @brief
 */
enum GenieStrand : uint8_t {
  kGenieStrandUnmappedUnknown = 0,
  kGenieStrandForward = 1,
  kGenieStrandReverse = 2,
  kGenieStrandCount = 3
};

/**
 * @brief
 */
enum GenieStrandStrict : uint8_t {
  kGenieStrandStrictForward = 0,
  kGenieStrandStrictReverse = 1,
  kGenieStrandStrictCount = 2
};

/**
 * @brief
 */
enum GenieStrandPaired : uint8_t {
  kGenieStrandPairedUnmappedUnmapped = 0,
  kGenieStrandPairedUnmappedForward = 1,
  kGenieStrandPairedUnmappedReverse = 2,
  kGenieStrandPairedForwardUnmapped = 3,
  kGenieStrandPairedReverseUnmapped = 4,
  kGenieStrandPairedForwardForward = 5,
  kGenieStrandPairedForwardReverse = 6,
  kGenieStrandPairedReverseForward = 7,
  kGenieStrandPairedReverseReverse = 8,
  kGenieStrandPairedCount = 9
};

/**
 * @brief
 */
enum GenieClipType : uint8_t {
  kGenieClipTypeSoft = 0,
  kGenieClipTypeHard = 1,
  kGenieClipTypeCount = 2
};

/**
 * @brief
 */
enum GenieClipTypeCombination : uint8_t {
  kGenieClipTypeCombinationNone = 0,
  kGenieClipTypeCombinationSoft = 1,
  kGenieClipTypeCombinationSoftHard = 2,
  kGenieClipTypeCombinationCount = 3
};

/**
 * @brief
 */
enum GeniePhredBins {
  kGeniePhredBins010 = 0,
  kGeniePhredBins1025 = 1,
  kGeniePhredBins2550 = 2,
  kGeniePhredBins5075 = 3,
  kGeniePhredBins7590 = 4,
  kGeniePhredBins90100 = 5,
  kGeniePhredBinsCount = 6
};

/**
 * @brief
 */
enum GenieTagType : uint8_t {
  kGenieTagTypeInt32 = 0,
  kGenieTagTypeString = 1,
  kGenieTagTypeUint8 = 2,
  kGenieTagTypeInt8 = 3,
  kGenieTagTypeUint16 = 4,
  kGenieTagTypeInt16 = 5,
  kGenieTagTypeUint32 = 6,
  kGenieTagTypeHex4 = 7,
  kGenieTagTypeFloat32 = 8,
  kGenieTagTypeFloat64 = 9
};

/**
 * @brief
 */
enum GenieRecordClass : uint8_t {
  kGenieRecordClassP = 0,
  kGenieRecordClassN = 1,
  kGenieRecordClassM = 2,
  kGenieRecordClassI = 3,
  kGenieRecordClassHm = 4,
  kGenieRecordClassU = 5,
  kGenieRecordClassCount = 6,
};

/**
 * @brief
 */
enum GenieAlphabetAcgnt : uint8_t {
  kGenieAlphabetAcgntA = 0,
  kGenieAlphabetAcgntC = 1,
  kGenieAlphabetAcgntG = 2,
  kGenieAlphabetAcgntN = 3,
  kGenieAlphabetAcgntT = 4,
  kGenieAlphabetAcgntCount = 5,
};

/**
 * @brief
 */
struct GenieHierarchy {
  uint64_t dataset_groups_count; /*!< @brief */
  uint64_t* dataset_group_id;    /*!< @brief [datasetGroupsCount] */
  uint64_t* datasets_count;      /*!< @brief [datasetGroupsCount] */
  uint64_t** dataset_id; /*!< @brief [datasetGroupsCount][datasetsCount[i]] */
};

/**
 * @brief
 */
struct GenieMpeggRecord {
  void* tmp;
  /* TODO(muenteferi) implement */
};

/**
 * @brief
 */
struct GenieGenTag {
  char key[2];       /*!< @brief */
  GenieTagType type; /*!< @brief */
  uint16_t length;   /*!< @brief */
  union {
    int32_t i32;
    char* c_string;
    uint8_t u8;
    int8_t i8;
    uint16_t u16;
    int16_t i16;
    uint32_t u32;
    uint8_t hex;
    float f32;
    double f64;
  } value; /*!< @brief */
};

/**
 * @brief
 */
struct GenieGenAux {
  uint8_t number_of_tags;  /*!< @brief */
  GenieGenTag* aux_fields; /*!< @brief [numberOfTags] */
};

/**
 * @brief
 */
struct GenieGenAuxRecord {
  uint8_t number_of_gen_aux; /*!< @brief */
  GenieGenAux* aux_set;      /*!< @brief [numberOfGenAux] */
};

/**
 * @brief
 */
struct GenieRecords {
  uint64_t dataset_group_id;   /*!< @brief */
  uint64_t dataset_id;         /*!< @brief */
  uint64_t records_count;      /*!< @brief */
  GenieMpeggRecord* records;   /*!< @brief [recordsCount] */
  GenieGenAuxRecord* aux_info; /*!< @brief [recordsCount] */
};

/**
 * @brief
 */
struct GenieReference {
  uint64_t seq_count;  /*!< @brief */
  char** seq_name;     /*!< @brief [seqCount] \0 */
  uint64_t* seq_start; /*!< @brief [seqCount] */
  uint64_t* seq_end;   /*!< @brief [seqCount] */
  char** ref_sequence; /*!< @brief [seqCount] \0 */
};

/**
 * @brief
 */
struct GenieRegionProtection {
  char* sequence_name; /*!< @brief \0 */
  uint64_t start_pos;  /*!< @brief */
  uint64_t end_pos;    /*!< @brief */
  uint64_t class_id;   /*!< @brief */
  uint64_t num_keys;   /*!< @brief */
  char** key_name;     /*!< @brief [numKeys] \0 */
};

/**
 * @brief
 */
struct GenieSimpleSegmentStatistics {
  uint64_t reads_number;                        /*!< @brief */
  uint64_t* segments_number_reads_distribution; /*!< @brief [max_segments] */
  uint64_t quality_check_failed_reads_number;   /*!< @brief */
  uint64_t segment_length[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t mapped_strand_segment_distribution[kGenieStrandCount];
  /* NOLINT */                     /*!< @brief */
  uint64_t properly_paired_number; /*!< @brief */
  uint64_t mapped_strand_pair_distribution
      [GenieStrandPaired::kGenieStrandPairedCount];
  /* NOLINT */             /*!< @brief */
  uint64_t max_alignments; /*!< @brief */
  /**  @brief [maxAlignments + 1] */
  uint64_t* multiple_alignment_segment_distribution;
  uint64_t coverage[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t weighted_coverage[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t errors_number[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t substitutions_number[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t insertions_number[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t insertions_length[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t deletions_number[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t deletions_length[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t splices_number[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t splices_length[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t alignment_score[kGenieStatisticsIndexCount];
  /* NOLINT */ /*!< @brief */
  uint64_t class_segment_distribution[kGenieRecordClassCount];
  /* NOLINT */ /*!< @brief */
  uint64_t
      clipped_segment_distribution[kGenieClipTypeCombinationCount]; /* NOLINT
                                                                     */
  /*!< @brief */
  uint64_t optical_duplicates_number; /*!< @brief */
  uint64_t chimeras_number;           /*!< @brief */
};

/**
 * @brief
 */
struct genie_ExtendedSegmentStatistics {
  uint64_t*
      mapped_bases_number_distribution;   /*!< @brief [segmentLength[1] + 1] */
  uint64_t* coverage;                     /*!< @brief [endPos – startPos + 1] */
  uint64_t* weighted_coverage;            /*!< @brief [endPos – startPos + 1] */
  uint64_t* errors_number_distribution;   /*!< @brief [errorsNumber[1] + 1] */
  uint64_t* errors_position_distribution; /*!< @brief [segmentLength[1]] */
  /** @brief [substitutionsNumber[1] + 1] */
  uint64_t* substitutions_number_distribution;
  uint64_t* substitutions_transition_distribution
      [kGenieAlphabetAcgntCount]
      [GenieAlphabetAcgnt::kGenieAlphabetAcgntCount]; /* NOLINT */
  /*!< @brief [segmentLength[1]] */
  uint64_t*
      substitutions_position_distribution; /*!< @brief [segmentLength[1]] */
  uint64_t*
      insertions_number_distribution; /*!< @brief [insertionsNumber[1] + 1] */
  uint64_t*
      insertions_length_distribution; /*!< @brief [insertionsLength[1] + 1] */
  uint64_t* insertions_position_distribution; /*!< @brief [segmentLength[1]] */
  uint64_t*
      deletions_number_distribution; /*!< @brief [deletionsNumber[1] + 1] */
  uint64_t*
      deletions_length_distribution; /*!< @brief [deletionsLength[1] + 1] */
  uint64_t* deletions_position_distribution; /*!< @brief [segmentLength[1]] */
  uint64_t* splices_number_distribution[kGenieStrandCount]; /* NOLINT
                                                             */
  /*!< @brief [splicesNumber[1] + 1], */
  uint64_t* splices_length_distribution[kGenieStrandCount]; /* NOLINT
                                                             */
  /*!< @brief [splicesLength[1] + 1], */
  uint64_t* splices_position_distribution[kGenieStrandCount]; /* NOLINT
                                                               */
  /*!< @brief [segmentLength[1]], */
  /** @brief [alignmentScore[1] + 1] */
  uint64_t* alignment_score_value_distribution;
  uint64_t* alignment_score_segment_length_distribution
      [GenieStatisticsIndex::kGenieStatisticsIndexCount]; /* NOLINT */
  /*!< @brief [segmentLength[1]], */
  uint64_t* bases_position_distribution[kGenieAlphabetAcgntCount];
  /* NOLINT */ /*!< @brief [segmentLength[1]] */
  /** @brief GC content in percent 0-100 */
  int64_t gc_content_value_distribution[101];
  uint64_t max_quality_score;           /*!< @brief */
  uint64_t* quality_score_distribution; /*!< @brief [maxQualityScore + 1] */
  uint64_t* quality_score_position_distribution
      [GenieStatisticsIndex::kGenieStatisticsIndexCount]; /* NOLINT */
  /*!< @brief [segmentLength[1]] */
  uint64_t* quality_score_position_percentiles_distribution
      [GeniePhredBins::kGeniePhredBinsCount]; /* NOLINT */
                                              /*!< @brief [segmentLength[1]], */
};

/**
 * @brief
 */
struct GenieAdvancedSegmentStatistics {
  GenieSimpleSegmentStatistics
      simple_statistics[kGenieStrandStrictCount]; /* NOLINT
                                                   */
  /*!< @brief */
  genie_ExtendedSegmentStatistics
      extended_statistics[kGenieStrandStrictCount]; /* NOLINT
                                                     */
                                                    /*!< @brief */
};

/**
 * @brief
 */
struct GenieSimpleFilter {
  uint64_t number_of_groups; /*!< @brief */
  char** group_names;        /*!< @brief [numberOfGroups] \0 */
  bool class_id[kGenieRecordClassCount];
  /* NOLINT */          /*!< @brief */
  uint64_t sequence_id; /*!< @brief */
  uint64_t start_pos;   /*!< @brief */
  uint64_t end_pos;     /*!< @brief */
  bool single_ends_strand[kGenieStrandCount];
  /* NOLINT */ /*!< @brief */
  bool paired_ends_strand[kGenieStrandPairedCount];
  /* NOLINT */ /*!< @brief */
  bool include_clipped_reads[kGenieClipTypeCount];
  /* NOLINT */                       /*!< @brief genie_clips */
  bool include_multiple_alignments;  /*!< @brief */
  bool include_optical_duplicates;   /*!< @brief */
  bool include_quality_check_failed; /*!< @brief */
  bool include_aux_records;          /*!< @brief */
  bool include_read_names;           /*!< @brief */
  bool include_quality_values;       /*!< @brief */
  bool mismatches_include_ns;        /*!< @brief */
};

/**
 * @brief
 */
struct GenieRangeFloat {
  float min; /*!< @brief */
  float max; /*!< @brief */
};

/**
 * @brief
 */
struct GenieRangeInt {
  uint64_t min; /*!< @brief */
  uint64_t max; /*!< @brief */
};

/**
 * @brief
 */
struct GenieSegmentFilter {
  bool filter_scope[kGenieSegmentTypeCount];
  /* NOLINT */                                      /*!< @brief */
  GenieRangeInt mapped_bases_range;                 /*!< @brief */
  GenieRangeFloat mapped_fraction_range;            /*!< @brief */
  GenieRangeInt errors_range;                       /*!< @brief */
  GenieRangeFloat errors_fraction_range;            /*!< @brief */
  GenieRangeInt substitutions_range;                /*!< @brief */
  GenieRangeFloat substitutions_fraction_range;     /*!< @brief */
  GenieRangeInt insertions_range;                   /*!< @brief */
  GenieRangeFloat insertions_fraction_range;        /*!< @brief */
  GenieRangeInt insertions_length_range;            /*!< @brief */
  GenieRangeFloat insertions_length_fraction_range; /*!< @brief */
  GenieRangeInt deletions_range;                    /*!< @brief */
  GenieRangeFloat deletions_fraction_range;         /*!< @brief */
  GenieRangeInt deletions_length_range;             /*!< @brief */
  GenieRangeFloat deletions_length_fraction_range;  /*!< @brief */
  GenieRangeInt splices_range;                      /*!< @brief */
  GenieRangeFloat splices_fraction_range;           /*!< @brief */
  GenieRangeInt splices_length_range;               /*!< @brief */
  int splices_direction_as_end;                     /*!< @brief */
  GenieRangeFloat alignment_score_range;            /*!< @brief */
  GenieRangeInt quality_score_range;                /*!< @brief */
};

/**
 * @brief
 */
struct GenieAdvancedFilter {
  GenieSimpleFilter filter;            /*!< @brief */
  uint64_t segment_filters_count;      /*!< @brief */
  GenieSegmentFilter* segment_filters; /*!< @brief [segmentFiltersCount] */
};

/**
 * @brief
 * @param output_hierarchy
 * @return
 */
GenieReturnCode GenieGetHierarchy(GenieHierarchy** output_hierarchy);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param filter
 * @param output_records
 * @return
 */
GenieReturnCode GenieGetDataBySimpleFilter(uint64_t dataset_group_id,
                                           uint64_t dataset_id,
                                           const GenieSimpleFilter* filter,
                                           GenieRecords** output_records);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param filter
 * @param output_records
 * @return
 */
GenieReturnCode GenieGetDataByAdvancedFilter(uint64_t dataset_group_id,
                                             uint64_t dataset_id,
                                             const GenieAdvancedFilter* filter,
                                             GenieRecords** output_records);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param signature
 * @param output_records
 * @return
 */
GenieReturnCode GenieGetDataBySignature(uint64_t dataset_group_id,
                                        uint64_t dataset_id,
                                        const char* signature,
                                        GenieRecords** output_records);

/**
 * @brief
 * @param dataset_group_id
 * @param label_id
 * @param output_records
 * @return
 */
GenieReturnCode GenieGetDataByLabel(uint64_t dataset_group_id,
                                    const char* label_id,
                                    GenieRecords** output_records);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param output_metadata
 * @return
 */
GenieReturnCode GenieGetMetadataFields(uint64_t dataset_group_id,
                                       uint64_t dataset_id,
                                       char*** output_metadata);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param field_name
 * @param output_metadata
 * @return
 */
GenieReturnCode GenieGetMetadataContent(uint64_t dataset_group_id,
                                        uint64_t dataset_id,
                                        const char* field_name,
                                        char** output_metadata);

/**
 * @brief
 * @param dataset_group_id
 * @param output_protection
 * @return
 */
GenieReturnCode GenieGetDatasetGroupProtection(uint64_t dataset_group_id,
                                               char** output_protection);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param output_protection
 * @return
 */
GenieReturnCode GenieGetDatasetProtection(uint64_t dataset_group_id,
                                          uint64_t dataset_id,
                                          char** output_protection);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param sequence_id
 * @param start_pos
 * @param end_pos
 * @param output_protection
 * @return
 */
GenieReturnCode GenieGetDatasetRegionProtection(
    uint64_t dataset_group_id, uint64_t dataset_id, uint64_t sequence_id,
    uint64_t start_pos, uint64_t end_pos,
    GenieRegionProtection** output_protection);
/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param include_sequences
 * @param output_reference
 * @return
 */
GenieReturnCode GenieGetDatasetReference(
    uint64_t dataset_group_id, uint64_t dataset_id, bool include_sequences,
    const GenieReference* output_reference);
/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param sequence_id
 * @param start_pos
 * @param end_pos
 * @param max_segments
 * @param output_statistics
 * @return
 */
GenieReturnCode GenieGetSimpleStatistics(
    uint64_t dataset_group_id, uint64_t dataset_id, uint64_t sequence_id,
    uint64_t start_pos, uint64_t end_pos, const uint64_t* max_segments,
    GenieSimpleSegmentStatistics** output_statistics);

/**
 * @brief
 * @param dataset_group_id
 * @param dataset_id
 * @param sequence_id
 * @param start_pos
 * @param end_pos
 * @param max_segments
 * @param output_statistics
 * @return
 */
GenieReturnCode GenieGetAdvancedStatistics(
    uint64_t dataset_group_id, uint64_t dataset_id, uint64_t sequence_id,
    uint64_t start_pos, uint64_t end_pos, const uint64_t* max_segments,
    GenieAdvancedSegmentStatistics** output_statistics);

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/* ---------------------------------------------------------------------------*/

#endif /* SRC_GENIE_CORE_C_API_H_ */

/* ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/
