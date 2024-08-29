/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "genie/format/mgg/dataset_group.h"
#include "genie/format/mgg/dataset_group_metadata.h"
#include "genie/format/mgg/dataset_group_protection.h"
#include "genie/format/mgg/encapsulator/encapsulated_dataset.h"
#include "genie/format/mgg/label_list.h"
#include "genie/format/mgg/reference.h"
#include "genie/format/mgg/reference_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

/**
 * @brief
 */
struct EncapsulatedDatasetGroup {
    std::optional<genie::format::mgg::DatasetGroupMetadata> group_meta;          //!< @brief
    std::optional<genie::format::mgg::DatasetGroupProtection> group_protection;  //!< @brief
    std::vector<genie::format::mgg::Reference> references;                       //!< @brief
    std::vector<genie::format::mgg::ReferenceMetadata> reference_meta;           //!< @brief
    std::vector<std::unique_ptr<EncapsulatedDataset>> datasets;                  //!< @brief
    std::optional<genie::format::mgg::LabelList> labelList;                      //!< @brief

    /**
     * @brief
     * @param id
     */
    void patchID(uint8_t id);

    /**
     * @brief
     * @param version
     */
    void mergeMetadata(genie::core::MPEGMinorVersion version);

    /**
     * @brief
     * @param version
     */
    void mergeReferences(genie::core::MPEGMinorVersion version);

    /**
     * @brief
     */
    void mergeLabels();

    /**
     * @brief
     * @param input_files
     * @param version
     */
    EncapsulatedDatasetGroup(const std::vector<std::string>& input_files, genie::core::MPEGMinorVersion version);

    /**
     * @brief
     * @param version
     * @return
     */
    genie::format::mgg::DatasetGroup assemble(genie::core::MPEGMinorVersion version);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
