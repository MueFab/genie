/**
 * @file
 * @brief This file defines the `DatasetGroupProtection` class, representing protection data for a dataset group in the MPEG-G format.
 * @details The `DatasetGroupProtection` class encapsulates protection-related metadata for a dataset group. It allows for
 *          serialization and deserialization of the protection values, as well as modification of the group ID. This class
 *          extends the `GenInfo` base class and provides specific functionalities for managing the protection attributes.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_PROTECTION_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_PROTECTION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief This class represents protection information for a dataset group in MPEG-G.
 * @details The `DatasetGroupProtection` class stores a protection value string that indicates the level of protection
 *          or checksum applied to a dataset group. It includes methods for serialization, deserialization, and ID manipulation.
 */
class DatasetGroupProtection : public GenInfo {
 public:
    /**
     * @brief Compares the current object with another `GenInfo` object for equality.
     * @param info The other `GenInfo` object to compare with.
     * @return True if both objects are equal; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Retrieves the unique key representing this protection information.
     * @return The key string for this protection information.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Constructs a `DatasetGroupProtection` object by reading from a `BitReader` stream.
     * @param bitreader The input bit reader to read protection data from.
     * @param _version The MPEG-G version of the protection.
     */
    explicit DatasetGroupProtection(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version);

    /**
     * @brief Constructs a `DatasetGroupProtection` object with specified values.
     * @param _dataset_group_id The ID of the dataset group.
     * @param _dg_protection_value The protection value string.
     * @param _version The MPEG-G version of the protection.
     */
    DatasetGroupProtection(uint8_t _dataset_group_id, std::string _dg_protection_value,
                           genie::core::MPEGMinorVersion _version);

    /**
     * @brief Serializes the protection information into a `BitWriter`.
     * @param bitWriter The output bit writer to write the protection data to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the ID of the dataset group.
     * @return The ID of the dataset group.
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief Retrieves the protection value string.
     * @return The protection value string.
     */
    [[nodiscard]] const std::string& getProtection() const;

    /**
     * @brief Extracts the protection value from this object.
     * @return The extracted protection value string.
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
    std::string dg_protection_value;        //!< @brief The protection value string for the dataset group.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_PROTECTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
