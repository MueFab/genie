/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/api.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace api {

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionPartiallyAuthorized::Msg() const {
    return "Only partially authorized. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionNotAuthorized::Msg() const { return "Not authorized. " + genie::util::RuntimeException::Msg(); }

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionVerificationFailed::Msg() const {
    return "Signature verification failed. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDecryptionFailed::Msg() const {
    return "Decryption failed. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDatasetGroupNotFound::Msg() const {
    return "Dataset group not found. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionDatasetNotFound::Msg() const {
    return "Dataset not found. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionAccessUnitNotFound::Msg() const {
    return "Access unit not found. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionReferenceNotFound::Msg() const {
    return "Reference not found. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionSequenceNotFound::Msg() const {
    return "Sequence not found. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionMetadataFieldNotFound::Msg() const {
    return "Metadata field not found. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionMetadataInvalid::Msg() const {
    return "Metadata invalid. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionReferenceInvalid::Msg() const {
    return "Reference invalid. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionParameterInvalid::Msg() const {
    return "Parameter invalid. " + genie::util::RuntimeException::Msg();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ExceptionBitstreamInvalid::Msg() const {
    return "Bitstream invalid." + genie::util::RuntimeException::Msg();
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
    return std::vector<Records>();
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataByAdvancedFilter(uint64_t datasetGroupID, uint64_t datasetID,
                                                         const AdvancedFilter& filter) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)filter;
    // UTILS_DIE("Not implemented");
    return std::vector<Records>();
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataBySignature(uint64_t datasetGroupID, uint64_t datasetID,
                                                    const char* signature) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)signature;
    // UTILS_DIE("Not implemented");
    return std::vector<Records>();
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Records> GenieState::getDataByLabel(uint64_t datasetGroupID, const std::string& labelID) {
    (void)datasetGroupID;
    (void)labelID;
    // UTILS_DIE("Not implemented");
    return std::vector<Records>();
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> GenieState::getMetadataFields(uint64_t datasetGroupID, uint64_t datasetID) {
    (void)datasetGroupID;
    (void)datasetID;
    // UTILS_DIE("Not implemented");
    return std::vector<std::string>();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getMetadataContent(uint64_t datasetGroupID, uint64_t datasetID, const std::string& fieldName) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)fieldName;
    // UTILS_DIE("Not implemented");
    return std::string();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getDatasetGroupProtection(uint64_t datasetGroupID) {
    (void)datasetGroupID;
    // UTILS_DIE("Not implemented");
    return std::string();
}

// ---------------------------------------------------------------------------------------------------------------------

std::string GenieState::getDatasetProtection(uint64_t datasetGroupID, uint64_t datasetID) {
    (void)datasetGroupID;
    (void)datasetID;
    // UTILS_DIE("Not implemented");
    return std::string();
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
    return std::vector<RegionProtection>();
}

// ---------------------------------------------------------------------------------------------------------------------

OutReference GenieState::getDatasetReference(uint64_t datasetGroupID, uint64_t datasetID, bool includeSequences) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)includeSequences;
    // UTILS_DIE("Not implemented");
    return OutReference();
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
    return std::vector<SimpleSegmentStatistics>();
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
    return std::vector<AdvancedSegmentStatistics>();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace api
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
