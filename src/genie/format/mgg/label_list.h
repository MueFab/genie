/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the LabelList class for managing and storing lists of labels
 * in MPEG-G files.
 * @details This file contains the implementation of the `LabelList` class,
 * which is used to handle and store label information for a dataset group. The
 * class provides methods for serialization, deserialization, and metadata
 * extraction. Each LabelList is uniquely associated with a dataset group
 * identified by a group ID.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_LABEL_LIST_H_
#define SRC_GENIE_FORMAT_MGG_LABEL_LIST_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/label.h"
#include "genie/util/bit_reader.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a list of labels in an MPEG-G dataset group.
 * @details The LabelList class encapsulates a list of label entries, each
 * represented by a `Label` object. This class manages label metadata, provides
 * serialization and deserialization functionality, and allows labels to be
 *          associated with specific dataset groups through the
 * `dataset_group_ID`.
 */
class LabelList final : public GenInfo {
  uint8_t dataset_group_id_;  //!< @brief Identifier for the dataset group that
                             //!< this label list belongs to.
  std::vector<Label>
      labels_;  //!< @brief List of labels associated with this dataset group.

 public:
  /**
   * @brief Update the dataset group ID for this label list.
   * @param group_id New dataset group ID to be assigned.
   */
  void PatchId(uint8_t group_id);

  /**
   * @brief Retrieve the key identifier for the LabelList class.
   * @details The key uniquely identifies the type of the GenInfo-derived class.
   * @return String representation of the key.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Convert the LabelList into a vector of core metadata labels.
   * @param dataset Dataset ID associated with the label list.
   * @return A vector of core::meta::Label objects representing the label data.
   */
  std::vector<core::meta::Label> Decapsulate(uint16_t dataset);

  /**
   * @brief Compare this LabelList with another GenInfo object for equality.
   * @param info The other GenInfo object to compare.
   * @return True if both objects are equivalent, false otherwise.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Construct a new LabelList object with a specified dataset group ID.
   * @param ds_group_id The dataset group ID to associate with this LabelList.
   */
  explicit LabelList(uint8_t ds_group_id);

  /**
   * @brief Construct a new LabelList object by reading data from a BitReader.
   * @param reader The BitReader to read label list data from.
   */
  explicit LabelList(util::BitReader& reader);

  /**
   * @brief Retrieve the dataset group ID associated with this LabelList.
   * @return Dataset group ID.
   */
  [[nodiscard]] uint8_t GetDatasetGroupId() const;

  /**
   * @brief Retrieve the list of labels in this LabelList.
   * @return Reference to the vector of Label objects.
   */
  [[nodiscard]] const std::vector<Label>& GetLabels() const;

  /**
   * @brief Add a label to the list of labels.
   * @param l Label object to be added.
   */
  void AddLabel(Label l);

  /**
   * @brief Serialize the LabelList to a BitWriter.
   * @param bit_writer The BitWriter to serialize the data to.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Print debug information about the LabelList.
   * @param output Output stream to print to.
   * @param depth Current depth of the hierarchy (for structured printing).
   * @param max_depth Maximum depth to print (for structured printing).
   */
  void PrintDebug(std::ostream& output, uint8_t depth,
                   uint8_t max_depth) const override;

  /**
   * @brief Deserialize the LabelList from a BitReader.
   * @param reader The BitReader to read data from.
   * @param in_offset Flag indicating if the input data includes an offset.
   */
  void ReadBox(util::BitReader& reader, bool in_offset);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_LABEL_LIST_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
