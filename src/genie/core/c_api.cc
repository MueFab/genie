/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// -----------------------------------------------------------------------------

#include "genie/core/c_api.h"

// -----------------------------------------------------------------------------

extern "C" {

// -----------------------------------------------------------------------------

const char* GenieStrerror(const GenieReturnCode rc) {
  (void)rc;
  return "Not implemented";
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetHierarchy(GenieHierarchy** output_hierarchy) {
  (void)output_hierarchy;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDataBySimpleFilter(const uint64_t dataset_group_id,
                                           const uint64_t dataset_id,
                                           const GenieSimpleFilter* filter,
                                           GenieRecords** output_records) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)filter;
  (void)output_records;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDataByAdvancedFilter(const uint64_t dataset_group_id,
                                             const uint64_t dataset_id,
                                             const GenieAdvancedFilter* filter,
                                             GenieRecords** output_records) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)filter;
  (void)output_records;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDataBySignature(const uint64_t dataset_group_id,
                                        const uint64_t dataset_id,
                                        const char* signature,
                                        GenieRecords** output_records) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)signature;
  (void)output_records;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDataByLabel(const uint64_t dataset_group_id,
                                    const char* label_id,
                                    GenieRecords** output_records) {
  (void)dataset_group_id;
  (void)label_id;
  (void)output_records;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetMetadataFields(const uint64_t dataset_group_id,
                                       const uint64_t dataset_id,
                                       char*** output_metadata) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)output_metadata;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetMetadataContent(const uint64_t dataset_group_id,
                                        const uint64_t dataset_id,
                                        const char* field_name,
                                        char** output_metadata) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)field_name;
  (void)output_metadata;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDatasetGroupProtection(const uint64_t dataset_group_id,
                                               char** output_protection) {
  (void)dataset_group_id;
  (void)output_protection;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDatasetProtection(const uint64_t dataset_group_id,
                                          const uint64_t dataset_id,
                                          char** output_protection) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)output_protection;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDatasetRegionProtection(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const uint64_t sequence_id, const uint64_t start_pos,
    const uint64_t end_pos, GenieRegionProtection** output_protection) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)sequence_id;
  (void)start_pos;
  (void)end_pos;
  (void)output_protection;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetDatasetReference(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const bool include_sequences, const GenieReference* output_reference) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)include_sequences;
  (void)output_reference;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetSimpleStatistics(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const uint64_t sequence_id, const uint64_t start_pos,
    const uint64_t end_pos, const uint64_t* max_segments,
    GenieSimpleSegmentStatistics** output_statistics) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)sequence_id;
  (void)start_pos;
  (void)end_pos;
  (void)max_segments;
  (void)output_statistics;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

GenieReturnCode GenieGetAdvancedStatistics(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const uint64_t sequence_id, const uint64_t start_pos,
    const uint64_t end_pos, const uint64_t* max_segments,
    GenieAdvancedSegmentStatistics** output_statistics) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)sequence_id;
  (void)start_pos;
  (void)end_pos;
  (void)max_segments;
  (void)output_statistics;
  return kGenieReturnCodeGUnlistedError;
}

// -----------------------------------------------------------------------------

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
