/**
 * @file
 * @brief Defines the `AUInformation` class for handling access unit information metadata.
 * @details This class provides functionality to represent, read, and write access unit information
 *          within the MPEG-G mgg format. Access unit information is used to associate metadata with
 *          specific dataset groups and IDs in an MPEG-G data structure.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_AU_INFORMATION_H_
#define SRC_GENIE_FORMAT_MGG_AU_INFORMATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing access unit information in the MPEG-G mgg format.
 * @details The `AUInformation` class stores information related to access units, including version, dataset group ID,
 *          dataset ID, and other relevant data. It provides methods for reading, writing, and comparing information
 *          values to facilitate the handling of metadata within MPEG-G files.
 */
class AUInformation : public GenInfo {
 public:
    /**
     * @brief Retrieves the key identifier for this information box.
     * @return A reference to the string representing the key.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Constructor for reading information from a BitReader stream.
     * @param bitreader The BitReader to extract data from.
     * @param _version MPEG minor version to identify the format version.
     */
    explicit AUInformation(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version);

    /**
     * @brief Constructor for initializing access unit information with specific parameters.
     * @param _dataset_group_id Dataset group ID associated with the information.
     * @param _dataset_id Dataset ID associated with the information.
     * @param _au_information_value String value representing the information.
     * @param _version MPEG minor version of the data.
     */
    AUInformation(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _au_information_value,
                  genie::core::MPEGMinorVersion _version);

    /**
     * @brief Writes the information to a BitWriter stream.
     * @param bitWriter The BitWriter to write the data to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the dataset group ID associated with this information.
     * @return The dataset group ID as an 8-bit unsigned integer.
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief Retrieves the dataset ID associated with this information.
     * @return The dataset ID as a 16-bit unsigned integer.
     */
    [[nodiscard]] uint16_t getDatasetID() const;

    /**
     * @brief Retrieves the information string.
     * @return A constant reference to the string representing the information value.
     */
    [[nodiscard]] const std::string& getInformation() const;

    /**
     * @brief Compares the equality of two `AUInformation` objects.
     * @param info Reference to another `GenInfo` object for comparison.
     * @return True if both objects have identical values, otherwise false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Decapsulates and retrieves the information value as a string.
     * @return The string representing the information value.
     */
    std::string decapsulate();

 private:
    genie::core::MPEGMinorVersion version;  //!< @brief MPEG minor version of the information data.
    uint8_t dataset_group_id;               //!< @brief ID of the associated dataset group.
    uint16_t dataset_id;                    //!< @brief ID of the associated dataset.
    std::string au_information_value;       //!< @brief String representing the information value.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_AU_INFORMATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
