/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/c_api.h"

// ---------------------------------------------------------------------------------------------------------------------

extern "C" {

// ---------------------------------------------------------------------------------------------------------------------

const char* genie_strerror(const genie_ReturnCode rc) {
    (void)rc;
    return "Not implemented";
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getHierarchy(genie_Hierarchy** outputHierarchy) {
    (void)outputHierarchy;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDataBySimpleFilter(const uint64_t datasetGroupID, const uint64_t datasetID,
                                             const genie_SimpleFilter* filter, genie_Records** outputRecords) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)filter;
    (void)outputRecords;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDataByAdvancedFilter(const uint64_t datasetGroupID, const uint64_t datasetID,
                                               const genie_AdvancedFilter* filter, genie_Records** outputRecords) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)filter;
    (void)outputRecords;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDataBySignature(const uint64_t datasetGroupID, const uint64_t datasetID,
                                          const char* signature, genie_Records** outputRecords) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)signature;
    (void)outputRecords;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDataByLabel(const uint64_t datasetGroupID, const char* labelID,
                                      genie_Records** outputRecords) {
    (void)datasetGroupID;
    (void)labelID;
    (void)outputRecords;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getMetadataFields(const uint64_t datasetGroupID, const uint64_t datasetID,
                                         char*** outputMetadata) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)outputMetadata;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getMetadataContent(const uint64_t datasetGroupID, const uint64_t datasetID,
                                          const char* fieldName, char** outputMetadata) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)fieldName;
    (void)outputMetadata;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDatasetGroupProtection(const uint64_t datasetGroupID, char** outputProtection) {
    (void)datasetGroupID;
    (void)outputProtection;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDatasetProtection(const uint64_t datasetGroupID, const uint64_t datasetID,
                                            char** outputProtection) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)outputProtection;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDatasetRegionProtection(const uint64_t datasetGroup_ID, const uint64_t datasetID,
                                                  const uint64_t sequenceID, const uint64_t startPos,
                                                  const uint64_t endPos, genie_RegionProtection** outputProtection) {
    (void)datasetGroup_ID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    (void)outputProtection;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getDatasetReference(const uint64_t datasetGroupID, const uint64_t datasetID,
                                           const bool includeSequences, genie_Reference* outputReference) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)includeSequences;
    (void)outputReference;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getSimpleStatistics(const uint64_t datasetGroupID, const uint64_t datasetID,
                                           const uint64_t sequenceID, const uint64_t startPos, const uint64_t endPos,
                                           const uint64_t* maxSegments,
                                           genie_SimpleSegmentStatistics** outputStatistics) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    (void)maxSegments;
    (void)outputStatistics;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

genie_ReturnCode genie_getAdvancedStatistics(const uint64_t datasetGroupID, const uint64_t datasetID,
                                             const uint64_t sequenceID, const uint64_t startPos, const uint64_t endPos,
                                             const uint64_t* maxSegments,
                                             genie_AdvancedSegmentStatistics** outputStatistics) {
    (void)datasetGroupID;
    (void)datasetID;
    (void)sequenceID;
    (void)startPos;
    (void)endPos;
    (void)maxSegments;
    (void)outputStatistics;
    return genie_ReturnCode_G_UNLISTED_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
