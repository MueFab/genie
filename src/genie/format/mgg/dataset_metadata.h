/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief This file defines the `DatasetMetadata` class, representing metadata
 * for a dataset in an MPEG-G file.
 * @details The `DatasetMetadata` class is responsible for storing metadata
 * associated with a specific dataset, including details like group ID, dataset
 * ID, and metadata values. It supports operations such as reading from and
 * writing to bitstreams, patching IDs, and comparison with other metadata
 * objects.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_METADATA_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_METADATA_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class for managing dataset metadata in MPEG-G format.
 * @details This class encapsulates dataset metadata, supporting operations like
 * ID management, metadata extraction, and bitstream
 * serialization/deserialization. It also supports versioning using the MPEG
 * minor version to handle format-specific variations.
 */
class DatasetMetadata final : public GenInfo {
 public:
  /**
   * @brief Retrieves the unique key associated with this metadata object.
   * @details The key is used to uniquely identify this metadata entry within a
   * dataset group.
   * @return A constant reference to the key string.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Constructs a `DatasetMetadata` object by reading from a `BitReader`.
   * @param bitreader The bit reader to extract data from.
   * @param version The MPEG minor version of the metadata.
   */
  explicit DatasetMetadata(util::BitReader& bitreader,
                           core::MpegMinorVersion version);

  /**
   * @brief Constructs a `DatasetMetadata` object with specific parameters.
   * @param dataset_group_id The ID of the dataset group this metadata belongs
   * to.
   * @param dataset_id The ID of the dataset.
   * @param dg_metatdata_value The metadata value as a string.
   * @param version The MPEG minor version of the metadata.
   */
  DatasetMetadata(uint8_t dataset_group_id, uint16_t dataset_id,
                  std::string dg_metatdata_value,
                  core::MpegMinorVersion version);

  /**
   * @brief Writes the metadata to a `BitWriter`.
   * @param bit_writer The bit writer to output the metadata.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the dataset group ID.
   * @return The group ID of the dataset.
   */
  [[nodiscard]] uint8_t GetDatasetGroupId() const;

  /**
   * @brief Retrieves the unique dataset ID.
   * @return The ID of the dataset.
   */
  [[nodiscard]] uint16_t GetDatasetId() const;

  /**
   * @brief Retrieves the stored metadata value.
   * @return A constant reference to the metadata value string.
   */
  [[nodiscard]] const std::string& GetMetadata() const;

  /**
   * @brief Compares this `DatasetMetadata` object with another `GenInfo` object
   * for equality.
   * @param info The `GenInfo` object to compare against.
   * @return True if both objects are equal; otherwise, false.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Extracts and returns the encapsulated metadata value.
   * @return The decapsulated metadata value as a string.
   */
  std::string decapsulate();

  /**
   * @brief Updates the dataset group and dataset IDs of the metadata.
   * @param group_id The new group ID for the dataset.
   * @param set_id The new dataset ID.
   */
  void PatchId(uint8_t group_id, uint16_t set_id);

 private:
  core::MpegMinorVersion version_;  //!< @brief MPEG minor version to
                                          //!< handle format-specific behavior.
  uint8_t dataset_group_id_;  //!< @brief ID of the dataset group this metadata
                             //!< belongs to.
  uint16_t dataset_id_;       //!< @brief Unique ID of the dataset.
  std::string dg_metatdata_value_;  //!< @brief The metadata value associated
                                   //!< with the dataset.
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_METADATA_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
