/**
 * @file
 * @brief This file defines the `DatasetGroup` class, representing a group of datasets in the MPEG-G format.
 * @details The `DatasetGroup` class manages a group of datasets, including references, metadata, labels, and protection
 * data. It provides functionality to read, write, and manipulate dataset group information, ensuring consistency across
 * the associated datasets, references, and metadata. The class supports serialization and deserialization to and from
 * bitstreams.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/format/mgg/dataset.h"
#include "genie/format/mgg/dataset_group_header.h"
#include "genie/format/mgg/dataset_group_metadata.h"
#include "genie/format/mgg/dataset_group_protection.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/label_list.h"
#include "genie/format/mgg/reference.h"
#include "genie/format/mgg/reference_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a dataset group in MPEG-G.
 * @details The `DatasetGroup` class encapsulates the header, datasets, references, labels, metadata, and protection
 *          information for a group of datasets in the MPEG-G format. It allows for managing the various components of
 *          the dataset group and provides methods to serialize and deserialize the group data.
 */
class DatasetGroup : public GenInfo {
 private:
    std::optional<DatasetGroupHeader> header;            //!< @brief Optional header for the dataset group.
    std::vector<Reference> references;                   //!< @brief List of references associated with the group.
    std::vector<ReferenceMetadata> reference_metadatas;  //!< @brief List of reference metadata entries.
    std::optional<LabelList> labels;                     //!< @brief Optional list of labels for the dataset group.
    std::optional<DatasetGroupMetadata> metadata;        //!< @brief Optional metadata for the dataset group.
    std::optional<DatasetGroupProtection>
        protection;                //!< @brief Optional protection information for the dataset group.
    std::vector<Dataset> dataset;  //!< @brief List of datasets within the group.

    core::MPEGMinorVersion version;  //!< @brief MPEG version of the dataset group.

 public:
    /**
     * @brief Sets the metadata for the dataset group.
     * @param meta The metadata to set.
     */
    void setMetadata(DatasetGroupMetadata meta);

    /**
     * @brief Sets the protection information for the dataset group.
     * @param prot The protection information to set.
     */
    void setProtection(DatasetGroupProtection prot);

    /**
     * @brief Adds a reference to the dataset group.
     * @param r The reference to add.
     */
    void addReference(Reference r);

    /**
     * @brief Adds reference metadata to the dataset group.
     * @param r The reference metadata to add.
     */
    void addReferenceMeta(ReferenceMetadata r);

    /**
     * @brief Sets the label list for the dataset group.
     * @param l The label list to set.
     */
    void setLabels(LabelList l);

    /**
     * @brief Patches the dataset group ID with a new value.
     * @param groupID The new group ID to set.
     */
    void patchID(uint8_t groupID);

    /**
     * @brief Updates the reference ID in the dataset group.
     * @param _old The old reference ID.
     * @param _new The new reference ID.
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief Compares the current object with another `GenInfo` object for equality.
     * @param info The other `GenInfo` object to compare with.
     * @return True if both objects are equal; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Constructs a `DatasetGroup` object by reading from a `BitReader` stream.
     * @param reader The input bit reader to read dataset group information from.
     * @param _version The MPEG version for the dataset group.
     */
    explicit DatasetGroup(util::BitReader& reader, core::MPEGMinorVersion _version);

    /**
     * @brief Constructs a `DatasetGroup` object with specified values.
     * @param _ID The dataset group ID.
     * @param _version The dataset group version.
     * @param _mpeg_version The MPEG minor version.
     */
    DatasetGroup(uint8_t _ID, uint8_t _version, core::MPEGMinorVersion _mpeg_version);

    /**
     * @brief Adds a dataset to the dataset group.
     * @param ds The dataset to add.
     */
    void addDataset(Dataset ds);

    /**
     * @brief Retrieves the dataset group header.
     * @return A constant reference to the dataset group header.
     */
    [[nodiscard]] const DatasetGroupHeader& getHeader() const;

    /**
     * @brief Retrieves the list of references in the dataset group.
     * @return A constant reference to the list of references.
     */
    [[nodiscard]] const std::vector<Reference>& getReferences() const;

    /**
     * @brief Retrieves the list of references in the dataset group.
     * @return A modifiable reference to the list of references.
     */
    std::vector<Reference>& getReferences();

    /**
     * @brief Retrieves the list of reference metadata entries.
     * @return A constant reference to the list of reference metadata.
     */
    [[nodiscard]] const std::vector<ReferenceMetadata>& getReferenceMetadata() const;

    /**
     * @brief Retrieves the list of reference metadata entries.
     * @return A modifiable reference to the list of reference metadata.
     */
    std::vector<ReferenceMetadata>& getReferenceMetadata();

    /**
     * @brief Checks if the dataset group has a label list.
     * @return True if the dataset group has a label list; otherwise, false.
     */
    [[nodiscard]] bool hasLabelList() const;

    /**
     * @brief Retrieves the label list of the dataset group.
     * @return A constant reference to the label list.
     */
    [[nodiscard]] const LabelList& getLabelList() const;

    /**
     * @brief Retrieves the label list of the dataset group.
     * @return A modifiable reference to the label list.
     */
    LabelList& getLabelList();

    /**
     * @brief Checks if the dataset group has metadata.
     * @return True if the dataset group has metadata; otherwise, false.
     */
    [[nodiscard]] bool hasMetadata() const;

    /**
     * @brief Retrieves the metadata of the dataset group.
     * @return A constant reference to the metadata.
     */
    [[nodiscard]] const DatasetGroupMetadata& getMetadata() const;

    /**
     * @brief Retrieves the metadata of the dataset group.
     * @return A modifiable reference to the metadata.
     */
    DatasetGroupMetadata& getMetadata();

    /**
     * @brief Checks if the dataset group has protection information.
     * @return True if the dataset group has protection information; otherwise, false.
     */
    [[nodiscard]] bool hasProtection() const;

    /**
     * @brief Retrieves the protection information of the dataset group.
     * @return A constant reference to the protection information.
     */
    [[nodiscard]] const DatasetGroupProtection& getProtection() const;

    /**
     * @brief Retrieves the protection information of the dataset group.
     * @return A modifiable reference to the protection information.
     */
    DatasetGroupProtection& getProtection();

    /**
     * @brief Adds metadata to the dataset group.
     * @param md The metadata to add.
     */
    void addMetadata(DatasetGroupMetadata md);

    /**
     * @brief Adds protection information to the dataset group.
     * @param pr The protection information to add.
     */
    void addProtection(DatasetGroupProtection pr);

    /**
     * @brief Retrieves the list of datasets in the dataset group.
     * @return A constant reference to the list of datasets.
     */
    [[nodiscard]] const std::vector<Dataset>& getDatasets() const;

    /**
     * @brief Retrieves the list of datasets in the dataset group.
     * @return A modifiable reference to the list of datasets.
     */
    std::vector<Dataset>& getDatasets();

    /**
     * @brief Retrieves the unique key representing this dataset group.
     * @return The key string for this dataset group.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Serializes the dataset group into a `BitWriter`.
     * @param wr The output bit writer to write the dataset group to.
     */
    void box_write(util::BitWriter& wr) const override;

    /**
     * @brief Prints debugging information about this dataset group.
     * @param output The output stream to print the debugging information to.
     * @param depth The current depth of indentation.
     * @param max_depth The maximum depth of detail to print.
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

    /**
     * @brief Reads the dataset group from a `BitReader` stream.
     * @param reader The input bit reader to read the dataset group information from.
     * @param in_offset Indicates if the reading should consider offsets.
     */
    void read_box(util::BitReader& reader, bool in_offset);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
