/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `AUProtection` class for handling access unit protection
 * metadata.
 * @details This class provides the functionality to represent, read, and write
 * access unit protection information within the MPEG-G mgg format. Access unit
 * protection is used to secure specific dataset groups and IDs.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_AU_PROTECTION_H_
#define SRC_GENIE_FORMAT_MGG_AU_PROTECTION_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing access unit protection information in the MPEG-G
 * mgg format.
 * @details The `AUProtection` class stores protection information, including
 * version, dataset group ID, and dataset ID, and provides methods for reading,
 * writing, and comparing protection values.
 */
class AuProtection final : public GenInfo {
 public:
  /**
   * @brief Retrieves the key identifier for this protection box.
   * @return A reference to the string representing the key.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Constructor for reading protection information from a BitReader
   * stream.
   * @param bitreader The BitReader to extract data from.
   * @param version MPEG minor version to identify the format version.
   */
  explicit AuProtection(util::BitReader& bitreader,
                        core::MpegMinorVersion version);

  /**
   * @brief Constructor for initializing access unit protection with specific
   * parameters.
   * @param dataset_group_id Dataset group ID associated with the protection.
   * @param dataset_id Dataset ID associated with the protection.
   * @param au_protection_value String value representing the protection
   * information.
   * @param version MPEG minor version of the data.
   */
  AuProtection(uint8_t dataset_group_id, uint16_t dataset_id,
               std::string au_protection_value,
               core::MpegMinorVersion version);

  /**
   * @brief Writes the protection information to a BitWriter stream.
   * @param bit_writer The BitWriter to write the data to.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the dataset group ID associated with this protection.
   * @return The dataset group ID as an 8-bit unsigned integer.
   */
  [[nodiscard]] uint8_t GetDatasetGroupId() const;

  /**
   * @brief Retrieves the dataset ID associated with this protection.
   * @return The dataset ID as a 16-bit unsigned integer.
   */
  [[nodiscard]] uint16_t GetDatasetId() const;

  /**
   * @brief Retrieves the protection information string.
   * @return A constant reference to the string representing the protection
   * value.
   */
  [[nodiscard]] const std::string& GetInformation() const;

  /**
   * @brief Compares the equality of two `AUProtection` objects.
   * @param info Reference to another `GenInfo` object for comparison.
   * @return True if both objects have identical values, otherwise false.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Decapsulates and retrieves the protection value as a string.
   * @return The string representing the protection information.
   */
  std::string decapsulate();

 private:
  core::MpegMinorVersion
      version_;  //!< @brief MPEG minor version of the protection data.
  uint8_t dataset_group_id_;  //!< @brief ID of the associated dataset group.
  uint16_t dataset_id_;       //!< @brief ID of the associated dataset.
  std::string au_protection_value_;  //!< @brief String representing the
                                    //!< protection value.
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_AU_PROTECTION_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
