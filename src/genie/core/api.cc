/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/api.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::api {

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionPartiallyAuthorized::msg() const {
    return "Only partially authorized. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionNotAuthorized::msg() const { return "Not authorized. " + genie::util::RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionVerificationFailed::msg() const {
    return "Signature verification failed. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDecryptionFailed::msg() const {
    return "Decryption failed. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDatasetGroupNotFound::msg() const {
    return "Dataset group not found. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDatasetNotFound::msg() const {
    return "Dataset not found. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionAccessUnitNotFound::msg() const {
    return "Access unit not found. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionReferenceNotFound::msg() const {
    return "Reference not found. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionSequenceNotFound::msg() const {
    return "Sequence not found. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionMetadataFieldNotFound::msg() const {
    return "Metadata field not found. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionMetadataInvalid::msg() const {
    return "Metadata invalid. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionReferenceInvalid::msg() const {
    return "Reference invalid. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionParameterInvalid::msg() const {
    return "Parameter invalid. " + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionBitstreamInvalid::msg() const {
    return "Bitstream invalid." + genie::util::RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

Hierarchy GenieState::getHierarchy() {
    // UTILS_DIE("Not implemented");
    return Hierarchy{};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataBySimpleFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                                       const SimpleFilter& filter) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)filter;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataByAdvancedFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                                         const AdvancedFilter& filter) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)filter;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataBySignature(uint64_t datasetGroupID, uint64_t datasetID,
                                                    const char* signature) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)signature;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataByLabel(uint64_t datasetGroupID, const std::string& labelID) {
    (void)datasetGroupID;
    (void)labelID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> GenieState::getMetadataFields(uint64_t datasetGroupID, uint64_t datasetID) {
    (void)datasetGroupID;
    (void)datasetID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getMetadataContent(uint64_t datasetGroupID, uint64_t datasetID, const std::string& fieldName) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)fieldName;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getDatasetGroupProtection(uint64_t datasetGroupID) {
    (void)datasetGroupID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getDatasetProtection(uint64_t datasetGroupID, uint64_t datasetID) {
    (void)datasetGroupID;
    (void)datasetID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<RegionProtection> GenieState::getDatasetRegionProtection(uint64_t datasetGroupID, uint64_t datasetID,
                                                                     uint64_t sequenceID, uint64_t startPos,
                                                                     uint64_t endPos) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

OutReference GenieState::getDatasetReference(uint64_t datasetGroupID, uint64_t datasetID, bool includeSequences) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)includeSequences;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<SimpleSegmentStatistics> GenieState::getSimpleStatistics(uint64_t datasetGroupID, uint64_t datasetID,
                                                                     uint64_t sequenceID, uint64_t startPos,
                                                                     uint64_t endPos) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<AdvancedSegmentStatistics> GenieState::getAdvancedStatistics(uint64_t datasetGroupID, uint64_t datasetID,
                                                                         uint64_t sequenceID, uint64_t startPos,
                                                                         uint64_t endPos) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::api

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
