/**
 * @file
 * @brief This file defines the `DatasetGroupMetadata` class, representing metadata for a dataset group in the MPEG-G format.
 * @details The `DatasetGroupMetadata` class provides functionality for storing, serializing, and managing metadata
 *          related to a dataset group. It allows for reading and writing metadata values, updating the group ID, and
 *          extracting the metadata as a string. The class extends the `GenInfo` base class to integrate with the
 *          overall GENIE file structure.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_METADATA_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing metadata for a dataset group in MPEG-G.
 * @details The `DatasetGroupMetadata` class encapsulates metadata for a dataset group, allowing for
 *          easy management and manipulation of these metadata values. The metadata can be serialized,
 *          deserialized, and modified as necessary.
 */
class DatasetGroupMetadata : public GenInfo {
 public:
    /**
     * @brief Compares the current object with another `GenInfo` object for equality.
     * @param info The other `GenInfo` object to compare with.
     * @return True if both objects are equal; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Retrieves the unique key representing this metadata information.
     * @return The key string for this metadata information.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Constructs a `DatasetGroupMetadata` object by reading from a `BitReader` stream.
     * @param bitreader The input bit reader to read metadata from.
     * @param _version The MPEG-G version of the metadata.
     */
    explicit DatasetGroupMetadata(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version);

    /**
     * @brief Constructs a `DatasetGroupMetadata` object with specified values.
     * @param _dataset_group_id The ID of the dataset group.
     * @param _dg_metatdata_value The metadata value string.
     * @param _version The MPEG-G version of the metadata.
     */
    DatasetGroupMetadata(uint8_t _dataset_group_id, std::string _dg_metatdata_value,
                         genie::core::MPEGMinorVersion _version);

    /**
     * @brief Serializes the metadata information into a `BitWriter`.
     * @param bitWriter The output bit writer to write the metadata to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the ID of the dataset group.
     * @return The ID of the dataset group.
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief Retrieves the metadata value string.
     * @return The metadata value string.
     */
    [[nodiscard]] const std::string& getMetadata() const;

    /**
     * @brief Extracts the metadata value from this object.
     * @return The extracted metadata value string.
     */
    std::string decapsulate();

    /**
     * @brief Updates the dataset group ID.
     * @param groupID The new group ID to set.
     */
    void patchID(uint8_t groupID);

 private:
    genie::core::MPEGMinorVersion version;  //!< @brief The version of the MPEG-G standard used.
    uint8_t dataset_group_id;               //!< @brief The ID of the dataset group.
    std::string dg_metatdata_value;         //!< @brief The metadata value string for the dataset group.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
