/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief This file defines the `DataSetMappingTableList` class, representing a
 * list of dataset mapping tables in the MPEG-G format.
 * @details The `DataSetMappingTableList` class is responsible for storing and
 * managing references to multiple dataset mapping tables, identified by their
 * unique IDs. It provides functionality to add, retrieve, and manipulate these
 * mappings, as well as to serialize and deserialize the object using bit
 * streams.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_LIST_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_LIST_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class for managing a list of dataset mapping tables in MPEG-G format.
 * @details This class encapsulates a set of dataset mapping table identifiers,
 * allowing the user to keep track of multiple mappings within a single dataset
 * group. It supports operations such as adding new mappings, retrieving the
 * list of mappings, and serializing/deserializing to bitstreams.
 */
class DataSetMappingTableList : public GenInfo {
 private:
  uint8_t dataset_group_ID;  //!< @brief ID of the dataset group this mapping
                             //!< list belongs to.
  std::vector<uint16_t>
      dataset_mapping_table_SID;  //!< @brief List of mapping table IDs
                                  //!< associated with this group.

 public:
  /**
   * @brief Compares this `DataSetMappingTableList` object with another
   * `GenInfo` object for equality.
   * @param info The `GenInfo` object to compare against.
   * @return True if both objects are equal; otherwise, false.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Retrieves the unique key associated with this mapping table list.
   * @details The key is used to uniquely identify this mapping table list
   * within a dataset group.
   * @return A constant reference to the key string.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Constructs a new `DataSetMappingTableList` object with a given
   * dataset group ID.
   * @param ds_group_id The ID of the dataset group this mapping table list
   * belongs to.
   */
  explicit DataSetMappingTableList(uint8_t ds_group_id);

  /**
   * @brief Constructs a new `DataSetMappingTableList` object by reading from a
   * `BitReader`.
   * @param reader The bit reader to extract data from.
   */
  explicit DataSetMappingTableList(util::BitReader& reader);

  /**
   * @brief Adds a new dataset mapping table identifier to the list.
   * @param sid The dataset mapping table ID to add.
   */
  void AddDatasetMappingTableSid(uint16_t sid);

  /**
   * @brief Serializes the mapping table list to a `BitWriter`.
   * @param bit_writer The bit writer to output the mapping table list.
   */
  void BoxWrite(genie::util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the dataset group ID associated with this mapping table
   * list.
   * @return The ID of the dataset group.
   */
  [[nodiscard]] uint8_t GetDatasetGroupId() const;

  /**
   * @brief Retrieves the list of dataset mapping table identifiers.
   * @return A constant reference to the list of mapping table IDs.
   */
  [[nodiscard]] const std::vector<uint16_t>& GetDatasetMappingTableSiDs() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_LIST_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
