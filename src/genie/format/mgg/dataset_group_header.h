/**
 * @file
 * @brief This file defines the `DatasetGroupHeader` class, representing the header information for a dataset group in the MPEG-G format.
 * @details The `DatasetGroupHeader` class is responsible for storing, serializing, and managing header information for a dataset group.
 *          This class includes attributes such as the group ID, version number, and a list of dataset IDs that belong to this group.
 *          It extends the `GenInfo` base class and provides functionality for reading and writing this header information,
 *          as well as modifying the group ID and dataset IDs as necessary.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing the header information for a dataset group in MPEG-G.
 * @details The `DatasetGroupHeader` class encapsulates header information for a dataset group, allowing
 *          for easy management and manipulation of the header values. The class supports operations such as
 *          serialization, deserialization, and modification of the group ID and dataset IDs.
 */
class DatasetGroupHeader : public GenInfo {
 private:
    uint8_t ID;                         //!< @brief The ID of the dataset group.
    uint8_t version;                    //!< @brief The version of the dataset group header.
    std::vector<uint16_t> dataset_IDs;  //!< @brief List of dataset IDs belonging to this group.

 public:
    /**
     * @brief Updates the ID of the dataset group.
     * @param groupID The new group ID to set.
     */
    void patchID(uint8_t groupID);

    /**
     * @brief Compares the current object with another `GenInfo` object for equality.
     * @param info The other `GenInfo` object to compare with.
     * @return True if both objects are equal; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Default constructor for creating an empty `DatasetGroupHeader` object.
     */
    DatasetGroupHeader();

    /**
     * @brief Constructs a `DatasetGroupHeader` object with specified values.
     * @param _id The ID of the dataset group.
     * @param _version The version of the dataset group.
     */
    DatasetGroupHeader(uint8_t _id, uint8_t _version);

    /**
     * @brief Retrieves the unique key representing this dataset group header.
     * @return The key string for this dataset group header.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Constructs a `DatasetGroupHeader` object by reading from a `BitReader` stream.
     * @param reader The input bit reader to read header information from.
     */
    explicit DatasetGroupHeader(genie::util::BitReader& reader);

    /**
     * @brief Retrieves the ID of the dataset group.
     * @return The ID of the dataset group.
     */
    [[nodiscard]] uint8_t getID() const;

    /**
     * @brief Sets the ID of the dataset group.
     * @param _ID The new ID to set for the dataset group.
     */
    void setID(uint8_t _ID);

    /**
     * @brief Retrieves the version of the dataset group header.
     * @return The version of the dataset group header.
     */
    [[nodiscard]] uint8_t getVersion() const;

    /**
     * @brief Retrieves the list of dataset IDs belonging to this group.
     * @return A vector of dataset IDs.
     */
    [[nodiscard]] const std::vector<uint16_t>& getDatasetIDs() const;

    /**
     * @brief Adds a new dataset ID to the group.
     * @param _id The dataset ID to add.
     */
    void addDatasetID(uint8_t _id);

    /**
     * @brief Serializes the header information into a `BitWriter`.
     * @param writer The output bit writer to write the header information to.
     */
    void box_write(genie::util::BitWriter& writer) const override;

    /**
     * @brief Prints debugging information about this dataset group header.
     * @param output The output stream to print the debugging information to.
     * @param depth The current depth of indentation.
     * @param max_depth The maximum depth of detail to print.
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
