/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief This file defines the `DatasetGroupHeader` class, representing the
 * header information for a dataset group in the MPEG-G format.
 * @details The `DatasetGroupHeader` class is responsible for storing,
 * serializing, and managing header information for a dataset group. This class
 * includes attributes such as the group ID, version number, and a list of
 * dataset IDs that belong to this group. It extends the `GenInfo` base class
 * and provides functionality for reading and writing this header information,
 * as well as modifying the group ID and dataset IDs as necessary.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_HEADER_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing the header information for a dataset group in
 * MPEG-G.
 * @details The `DatasetGroupHeader` class encapsulates header information for a
 * dataset group, allowing for easy management and manipulation of the header
 * values. The class supports operations such as serialization, deserialization,
 * and modification of the group ID and dataset IDs.
 */
class DatasetGroupHeader final : public GenInfo {
  uint8_t id_;       //!< @brief The ID of the dataset group.
  uint8_t version_;  //!< @brief The version of the dataset group header.
  std::vector<uint16_t>
      dataset_i_ds_;  //!< @brief List of dataset IDs belonging to this group.

 public:
  /**
   * @brief Updates the ID of the dataset group.
   * @param group_id The new group ID to set.
   */
  void PatchId(uint8_t group_id);

  /**
   * @brief Compares the current object with another `GenInfo` object for
   * equality.
   * @param info The other `GenInfo` object to compare with.
   * @return True if both objects are equal; otherwise, false.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Default constructor for creating an empty `DatasetGroupHeader`
   * object.
   */
  DatasetGroupHeader();

  /**
   * @brief Constructs a `DatasetGroupHeader` object with specified values.
   * @param id The ID of the dataset group.
   * @param version The version of the dataset group.
   */
  DatasetGroupHeader(uint8_t id, uint8_t version);

  /**
   * @brief Retrieves the unique key representing this dataset group header.
   * @return The key string for this dataset group header.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Constructs a `DatasetGroupHeader` object by reading from a
   * `BitReader` stream.
   * @param reader The input bit reader to read header information from.
   */
  explicit DatasetGroupHeader(util::BitReader& reader);

  /**
   * @brief Retrieves the ID of the dataset group.
   * @return The ID of the dataset group.
   */
  [[nodiscard]] uint8_t GetId() const;

  /**
   * @brief Sets the ID of the dataset group.
   * @param id The new ID to set for the dataset group.
   */
  void SetId(uint8_t id);

  /**
   * @brief Retrieves the version of the dataset group header.
   * @return The version of the dataset group header.
   */
  [[nodiscard]] uint8_t GetVersion() const;

  /**
   * @brief Retrieves the list of dataset IDs belonging to this group.
   * @return A vector of dataset IDs.
   */
  [[nodiscard]] const std::vector<uint16_t>& GetDatasetIDs() const;

  /**
   * @brief Adds a new dataset ID to the group.
   * @param id The dataset ID to add.
   */
  void AddDatasetId(uint8_t id);

  /**
   * @brief Serializes the header information into a `BitWriter`.
   * @param writer The output bit writer to write the header information to.
   */
  void BoxWrite(util::BitWriter& writer) const override;

  /**
   * @brief Prints debugging information about this dataset group header.
   * @param output The output stream to print the debugging information to.
   * @param depth The current depth of indentation.
   * @param max_depth The maximum depth of detail to print.
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_HEADER_H_

// -----------------------------------------------------------------------------
