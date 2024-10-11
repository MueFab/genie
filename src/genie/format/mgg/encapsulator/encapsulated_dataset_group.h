/**
 * @file
 * @brief Defines the `EncapsulatedDatasetGroup` structure for managing groups of encapsulated MPEG-G datasets.
 *
 * The `EncapsulatedDatasetGroup` class is part of the MPEG-G encapsulation module and is used to manage a group of
 * encapsulated datasets along with their associated metadata, references, and other group-level information.
 * It provides methods to merge metadata and references, patch dataset IDs, and assemble a complete `DatasetGroup`
 * object from individual components.
 *
 * @copyright
 * This file is part of GENIE. See LICENSE and/or https://github.com/MueFab/genie for more details.
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
 * @brief Encapsulates a group of MPEG-G datasets with shared metadata and reference information.
 *
 * The `EncapsulatedDatasetGroup` class manages a collection of encapsulated datasets along with group-level metadata,
 * protection information, and shared references. This structure allows the manipulation and assembly of complex MPEG-G
 * dataset groupings and supports operations for merging metadata, references, and labels across datasets.
 */
struct EncapsulatedDatasetGroup {
    std::optional<DatasetGroupMetadata> group_meta;          //!< @brief Optional group-level metadata.
    std::optional<DatasetGroupProtection> group_protection;  //!< @brief Optional protection information.
    std::vector<Reference> references;                       //!< @brief List of reference sequences used by the group.
    std::vector<ReferenceMetadata> reference_meta;           //!< @brief Metadata associated with each reference.
    std::vector<std::unique_ptr<EncapsulatedDataset>> datasets;  //!< @brief Collection of encapsulated datasets.
    std::optional<LabelList> labelList;                          //!< @brief Optional label list for the dataset group.

    /**
     * @brief Patches the group ID for all datasets and references in the group.
     *
     * This function updates the group ID of each dataset and reference within the group to ensure consistency.
     *
     * @param id The new group ID to assign.
     */
    void patchID(uint8_t id);

    /**
     * @brief Merges the metadata of all encapsulated datasets in the group.
     *
     * This function consolidates metadata from individual datasets into a single unified group metadata structure.
     * It also updates the metadata to be compliant with the specified MPEG-G version.
     *
     * @param version The MPEG-G minor version for the resulting metadata.
     */
    void mergeMetadata(genie::core::MPEGMinorVersion version);

    /**
     * @brief Merges reference information from all datasets in the group.
     *
     * This function consolidates reference sequences and metadata from individual datasets into a unified group-level
     * reference structure.
     *
     * @param version The MPEG-G minor version for the reference format.
     */
    void mergeReferences(genie::core::MPEGMinorVersion version);

    /**
     * @brief Merges labels from all encapsulated datasets in the group.
     *
     * Consolidates the labels from individual datasets into a single label list for the entire group.
     */
    void mergeLabels();

    /**
     * @brief Constructs an `EncapsulatedDatasetGroup` from a list of input files.
     *
     * This constructor reads and encapsulates multiple input files, creating a structured group of MPEG-G datasets.
     *
     * @param input_files A vector of paths to input files representing the datasets to be grouped.
     * @param version The MPEG-G minor version for the encapsulated format.
     * @throws `std::runtime_error` if any of the files cannot be opened or processed.
     */
    EncapsulatedDatasetGroup(const std::vector<std::string>& input_files, genie::core::MPEGMinorVersion version);

    /**
     * @brief Assembles a complete `DatasetGroup` object from the encapsulated components.
     *
     * This function combines all individual datasets, references, and metadata into a single `DatasetGroup` object,
     * which can be used for further processing or output.
     *
     * @param version The MPEG-G minor version for the resulting dataset group.
     * @return A `DatasetGroup` object representing the assembled group.
     */
    genie::format::mgg::DatasetGroup assemble(genie::core::MPEGMinorVersion version);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
