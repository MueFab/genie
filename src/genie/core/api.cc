/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/api.h"

#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core::api {

// -----------------------------------------------------------------------------

std::string ExceptionPartiallyAuthorized::Msg() const {
  return "Only partially authorized. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionNotAuthorized::Msg() const {
  return "Not authorized. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionVerificationFailed::Msg() const {
  return "Signature verification failed. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionDecryptionFailed::Msg() const {
  return "Decryption failed. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionDatasetGroupNotFound::Msg() const {
  return "Dataset group not found. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionDatasetNotFound::Msg() const {
  return "Dataset not found. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionAccessUnitNotFound::Msg() const {
  return "Access unit not found. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionReferenceNotFound::Msg() const {
  return "Reference not found. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionSequenceNotFound::Msg() const {
  return "Sequence not found. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionMetadataFieldNotFound::Msg() const {
  return "Metadata field not found. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionMetadataInvalid::Msg() const {
  return "Metadata invalid. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionReferenceInvalid::Msg() const {
  return "Reference invalid. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionParameterInvalid::Msg() const {
  return "Parameter invalid. " + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

std::string ExceptionBitstreamInvalid::Msg() const {
  return "Bitstream invalid." + RuntimeException::Msg();
}

// -----------------------------------------------------------------------------

Hierarchy GenieState::GetHierarchy() {
  // UTILS_DIE_("Not implemented");
  return Hierarchy{};
}

// -----------------------------------------------------------------------------

std::vector<Records> GenieState::GetDataBySimpleFilter(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const SimpleFilter& filter) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)filter;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::vector<Records> GenieState::GetDataByAdvancedFilter(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const AdvancedFilter& filter) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)filter;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::vector<Records> GenieState::GetDataBySignature(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const char* signature) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)signature;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::vector<Records> GenieState::GetDataByLabel(const uint64_t dataset_group_id,
                                                const std::string& label_id) {
  (void)dataset_group_id;
  (void)label_id;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::vector<std::string> GenieState::GetMetadataFields(
    const uint64_t dataset_group_id, const uint64_t dataset_id) {
  (void)dataset_group_id;
  (void)dataset_id;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::string GenieState::GetMetadataContent(const uint64_t dataset_group_id,
                                           const uint64_t dataset_id,
                                           const std::string& field_name) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)field_name;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::string GenieState::GetDatasetGroupProtection(
    const uint64_t dataset_group_id) {
  (void)dataset_group_id;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::string GenieState::GetDatasetProtection(const uint64_t dataset_group_id,
                                             const uint64_t dataset_id) {
  (void)dataset_group_id;
  (void)dataset_id;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::vector<RegionProtection> GenieState::GetDatasetRegionProtection(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const uint64_t sequence_id, const uint64_t start_pos,
    const uint64_t end_pos) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)sequence_id;
  (void)start_pos;
  (void)end_pos;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

OutReference GenieState::GetDatasetReference(const uint64_t dataset_group_id,
                                             const uint64_t dataset_id,
                                             const bool include_sequences) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)include_sequences;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::vector<SimpleSegmentStatistics> GenieState::GetSimpleStatistics(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const uint64_t sequence_id, const uint64_t start_pos,
    const uint64_t end_pos) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)sequence_id;
  (void)start_pos;
  (void)end_pos;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

std::vector<AdvancedSegmentStatistics> GenieState::GetAdvancedStatistics(
    const uint64_t dataset_group_id, const uint64_t dataset_id,
    const uint64_t sequence_id, const uint64_t start_pos,
    const uint64_t end_pos) {
  (void)dataset_group_id;
  (void)dataset_id;
  (void)sequence_id;
  (void)start_pos;
  (void)end_pos;
  // UTILS_DIE_("Not implemented");
  return {};
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::api

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
