/**
 * @file
 * @brief This file defines the `DatasetParameterSet` class for managing dataset parameter sets within an MPEG-G file.
 * @details The `DatasetParameterSet` class handles the encapsulation and manipulation of dataset parameters, such as
 *          encoding sets, parameter updates, and parent-child relationships between parameter sets. It supports
 *          serialization and deserialization of parameter sets and is aware of the MPEG-G version to manage
 * format-specific behavior.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PARAMETER_SET_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <string>
#include "genie/core/constants.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgg/dataset_parameterset/update_info.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a parameter set for a dataset in the MPEG-G format.
 * @details This class is responsible for storing and managing a parameter set that is associated with a specific
 *          dataset within an MPEG-G file. It supports operations like updating dataset IDs, encapsulating and
 *          decapsulating parameter sets, handling parameter updates, and writing the parameter set data to a bitstream.
 */
class DatasetParameterSet : public GenInfo {
 private:
    uint8_t dataset_group_id;         //!< @brief ID of the dataset group this parameter set belongs to.
    uint16_t dataset_id;              //!< @brief Unique ID of the dataset.
    uint8_t parameter_set_ID;         //!< @brief ID of the parameter set within the dataset.
    uint8_t parent_parameter_set_ID;  //!< @brief ID of the parent parameter set (for hierarchical structures).
    std::optional<dataset_parameterset::UpdateInfo>
        param_update;                            //!< @brief Optional update information for the parameter set.
    genie::core::parameter::EncodingSet params;  //!< @brief The set of encoding parameters for this dataset.
    core::MPEGMinorVersion version;              //!< @brief The MPEG minor version for format-specific behavior.

 public:
    /**
     * @brief Updates the dataset group and dataset IDs of the parameter set.
     * @param _groupID The new dataset group ID.
     * @param _setID The new dataset ID.
     */
    void patchID(uint8_t _groupID, uint16_t _setID);

    /**
     * @brief Compares this `DatasetParameterSet` object with another `GenInfo` object for equality.
     * @param info The `GenInfo` object to compare against.
     * @return True if both objects are equal; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Constructs a `DatasetParameterSet` object from a given `ParameterSet`.
     * @param _dataset_group_id The group ID of the dataset.
     * @param _dataset_id The unique ID of the dataset.
     * @param set The parameter set to encapsulate.
     * @param _version The MPEG minor version.
     */
    DatasetParameterSet(uint8_t _dataset_group_id, uint16_t _dataset_id, genie::core::parameter::ParameterSet set,
                        core::MPEGMinorVersion _version);

    /**
     * @brief Extracts and returns the encapsulated `ParameterSet` object.
     * @return The decapsulated `ParameterSet`.
     */
    genie::core::parameter::ParameterSet descapsulate();

    /**
     * @brief Constructs a `DatasetParameterSet` with specified properties.
     * @param _dataset_group_id The group ID of the dataset.
     * @param _dataset_id The unique ID of the dataset.
     * @param _parameter_set_ID The ID of this parameter set.
     * @param _parent_parameter_set_ID The ID of the parent parameter set.
     * @param ps The encoding set associated with this parameter set.
     * @param _version The MPEG minor version.
     */
    DatasetParameterSet(uint8_t _dataset_group_id, uint16_t _dataset_id, uint8_t _parameter_set_ID,
                        uint8_t _parent_parameter_set_ID, genie::core::parameter::EncodingSet ps,
                        core::MPEGMinorVersion _version);

    /**
     * @brief Constructs a `DatasetParameterSet` by reading from a `BitReader`.
     * @param reader The bit reader to extract the data from.
     * @param _version The MPEG minor version.
     * @param parameters_update_flag Indicates if parameters have been updated.
     */
    DatasetParameterSet(genie::util::BitReader& reader, core::MPEGMinorVersion _version, bool parameters_update_flag);

    /**
     * @brief Serializes the parameter set and writes it to a `BitWriter`.
     * @param writer The bit writer to output the data to.
     */
    void box_write(genie::util::BitWriter& writer) const override;

    /**
     * @brief Adds an update information to the parameter set.
     * @param update The update information to add.
     */
    void addParameterUpdate(dataset_parameterset::UpdateInfo update);

    /**
     * @brief Retrieves the key identifier for this `DatasetParameterSet`.
     * @return A constant reference to the key identifier string.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Retrieves the dataset group ID of this parameter set.
     * @return The dataset group ID.
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief Retrieves the unique dataset ID.
     * @return The dataset ID.
     */
    [[nodiscard]] uint16_t getDatasetID() const;

    /**
     * @brief Retrieves the ID of this parameter set.
     * @return The parameter set ID.
     */
    [[nodiscard]] uint8_t getParameterSetID() const;

    /**
     * @brief Retrieves the ID of the parent parameter set.
     * @return The parent parameter set ID.
     */
    [[nodiscard]] uint8_t getParentParameterSetID() const;

    /**
     * @brief Checks if the parameter set contains update information.
     * @return True if update information is present; otherwise, false.
     */
    [[nodiscard]] bool hasParameterUpdate() const;

    /**
     * @brief Retrieves the update information of the parameter set.
     * @return A constant reference to the update information.
     */
    [[nodiscard]] const dataset_parameterset::UpdateInfo& getParameterUpdate() const;

    /**
     * @brief Retrieves the encoding set associated with this parameter set.
     * @return A constant reference to the encoding set.
     */
    [[nodiscard]] const genie::core::parameter::EncodingSet& getEncodingSet() const;

    /**
     * @brief Moves and returns the encoding set associated with this parameter set.
     * @return An rvalue reference to the moved encoding set.
     */
    genie::core::parameter::EncodingSet&& moveParameterSet();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
