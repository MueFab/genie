/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief This file defines the `DatasetProtection` class, which represents the
 * protection information associated with a dataset in MPEG-G format.
 * @details The `DatasetProtection` class encapsulates information related to
 * the protection of datasets within an MPEG-G file, including dataset group ID,
 * dataset ID, and a protection value string. It provides methods for reading,
 * writing, and manipulating this protection data, as well as for handling
 * version-specific requirements of the MPEG-G format.
 * @copyright This file is part of Genie.
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PROTECTION_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PROTECTION_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing the protection information for a dataset in the
 * MPEG-G format.
 * @details This class encapsulates the protection information for a specific
 * dataset within an MPEG-G file. It includes attributes such as the dataset
 * group ID, dataset ID, and a protection value. The protection value is
 * typically used to verify the integrity or authenticity of the dataset. The
 * class provides methods for serializing and deserializing the protection
 * information, as well as for updating dataset identifiers.
 */
class DatasetProtection final : public GenInfo {
 public:
  /**
   * @brief Retrieves the key identifying the `DatasetProtection` object.
   * @details The key is used to uniquely identify this type of metadata box
   * within an MPEG-G file.
   * @return A constant reference to the string representing the key identifier.
   */
  [[nodiscard]] const std::string& GetKey() const override;

  /**
   * @brief Constructs a `DatasetProtection` object by reading from a bit
   * reader.
   * @param bitreader Reference to a `BitReader` to read the dataset protection
   * information from.
   * @param version The MPEG minor version to interpret the data correctly.
   */
  explicit DatasetProtection(util::BitReader& bitreader,
                             core::MpegMinorVersion version);

  /**
   * @brief Constructs a `DatasetProtection` object with the given parameters.
   * @param dataset_group_id The group ID of the dataset.
   * @param dataset_id The specific ID of the dataset.
   * @param dg_protection_value The protection value associated with the
   * dataset.
   * @param version The MPEG minor version to use.
   */
  DatasetProtection(uint8_t dataset_group_id, uint16_t dataset_id,
                    std::string dg_protection_value,
                    core::MpegMinorVersion version);

  /**
   * @brief Writes the dataset protection information to a `BitWriter`.
   * @param bit_writer Reference to a `BitWriter` to write the protection data
   * to.
   */
  void BoxWrite(util::BitWriter& bit_writer) const override;

  /**
   * @brief Retrieves the group ID of the dataset.
   * @return The group ID of the dataset.
   */
  [[nodiscard]] uint8_t GetDatasetGroupId() const;

  /**
   * @brief Retrieves the ID of the dataset.
   * @return The ID of the dataset.
   */
  [[nodiscard]] uint16_t GetDatasetId() const;

  /**
   * @brief Retrieves the protection value string of the dataset.
   * @details The protection value is typically used to verify the integrity of
   * the dataset.
   * @return A constant reference to the protection value string.
   */
  [[nodiscard]] const std::string& GetProtection() const;

  /**
   * @brief Compares the current `DatasetProtection` object with another
   * `GenInfo` object for equality.
   * @param info The `GenInfo` object to compare against.
   * @return True if both objects are equal; otherwise, false.
   */
  bool operator==(const GenInfo& info) const override;

  /**
   * @brief Extracts the protection value as a standard string.
   * @details This method decapsulates the protection information for external
   * use.
   * @return A string containing the protection value.
   */
  std::string Decapsulate();

  /**
   * @brief Updates the group ID and dataset ID of the protection information.
   * @param group_id The new group ID to set.
   * @param set_id The new dataset ID to set.
   */
  void PatchId(uint8_t group_id, uint16_t set_id);

 private:
  core::MpegMinorVersion
      version_;               //!< @brief The MPEG minor version of the format.
  uint8_t dataset_group_id_;  //!< @brief Group ID of the dataset.
  uint16_t dataset_id_;       //!< @brief Specific ID of the dataset.
  std::string dg_protection_value_;  //!< @brief Protection value associated
                                     //!< with the dataset.
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PROTECTION_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
