/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/api.h"
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::api {

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionPartiallyAuthorized::msg() const {
    return "Only partially authorized. " + RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionNotAuthorized::msg() const { return "Not authorized. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionVerificationFailed::msg() const {
    return "Signature verification failed. " + RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDecryptionFailed::msg() const { return "Decryption failed. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDatasetGroupNotFound::msg() const { return "Dataset group not found. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDatasetNotFound::msg() const { return "Dataset not found. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionAccessUnitNotFound::msg() const { return "Access unit not found. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionReferenceNotFound::msg() const { return "Reference not found. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionSequenceNotFound::msg() const { return "Sequence not found. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionMetadataFieldNotFound::msg() const {
    return "Metadata field not found. " + RuntimeException::msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionMetadataInvalid::msg() const { return "Metadata invalid. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionReferenceInvalid::msg() const { return "Reference invalid. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionParameterInvalid::msg() const { return "Parameter invalid. " + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionBitstreamInvalid::msg() const { return "Bitstream invalid." + RuntimeException::msg(); }

// ---------------------------------------------------------------------------------------------------------------------

Hierarchy GenieState::getHierarchy() {
    // UTILS_DIE("Not implemented");
    return Hierarchy{};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataBySimpleFilter(const uint64_t datasetGroupID, const uint64_t datasetID,
                                                       const SimpleFilter& filter) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)filter;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataByAdvancedFilter(const uint64_t datasetGroupID, const uint64_t datasetID,
                                                         const AdvancedFilter& filter) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)filter;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataBySignature(const uint64_t datasetGroupID, const uint64_t datasetID,
                                                    const char* signature) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)signature;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataByLabel(const uint64_t datasetGroupID, const std::string& labelID) {
    (void)datasetGroupID;
    (void)labelID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> GenieState::getMetadataFields(const uint64_t datasetGroupID, const uint64_t datasetID) {
    (void)datasetGroupID;
    (void)datasetID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getMetadataContent(const uint64_t datasetGroupID, const uint64_t datasetID,
                                           const std::string& fieldName) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)fieldName;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getDatasetGroupProtection(const uint64_t datasetGroupID) {
    (void)datasetGroupID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getDatasetProtection(const uint64_t datasetGroupID, const uint64_t datasetID) {
    (void)datasetGroupID;
    (void)datasetID;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<RegionProtection> GenieState::getDatasetRegionProtection(const uint64_t datasetGroupID,
                                                                     const uint64_t datasetID,
                                                                     const uint64_t sequenceID, const uint64_t startPos,
                                                                     const uint64_t endPos) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

OutReference GenieState::getDatasetReference(const uint64_t datasetGroupID, const uint64_t datasetID,
                                             const bool includeSequences) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)includeSequences;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<SimpleSegmentStatistics> GenieState::getSimpleStatistics(const uint64_t datasetGroupID,
                                                                     const uint64_t datasetID,
                                                                     const uint64_t sequenceID, const uint64_t startPos,
                                                                     const uint64_t endPos) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    // UTILS_DIE("Not implemented");
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<AdvancedSegmentStatistics> GenieState::getAdvancedStatistics(const uint64_t datasetGroupID,
                                                                         const uint64_t datasetID,
                                                                         const uint64_t sequenceID,
                                                                         const uint64_t startPos,
                                                                         const uint64_t endPos) {
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
