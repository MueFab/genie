/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_
#define SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>

#include "genie/core/meta/external_ref.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta::external_ref {

/**
 * @brief Represents an external ref in MPEG-G format.
 */
class Mpeg final : public ExternalRef {
  uint16_t
      external_dataset_group_id_;  //!< @brief Dataset group in external file
  uint16_t external_dataset_id_;   //!< @brief Dataset in the dataset group
  std::string ref_checksum_;       //!< @brief Checksum of the full reference

 public:
  /**
   * @brief Constructor from raw values
   * @param ref_uri URI to reference file
   * @param check Algorithm used for checksum
   * @param group_id Dataset group id in external file
   * @param id Dataset id in dataset group
   * @param checksum Checksum of full reference
   */
  Mpeg(std::string ref_uri, ChecksumAlgorithm check, uint16_t group_id,
       uint16_t id, std::string checksum);

  /**
   * @brief Constructor from json
   * @param json Json representation
   */
  explicit Mpeg(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const override;

  /**
   * @brief Return dataset group id in external file
   * @return Dataset group id in external file
   */
  [[nodiscard]] uint16_t GetGroupId() const;

  /**
   * @brief Return dataset ID in external file
   * @return Dataset ID in external file
   */
  [[nodiscard]] uint16_t GetId() const;

  /**
   * @brief Return checksum of full reference
   * @return Checksum of full reference
   */
  [[nodiscard]] const std::string& GetChecksum() const;

  /**
   * @brief
   * @return
   */
  std::string& GetChecksum();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<RefBase> Clone() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::external_ref

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_EXTERNAL_REF_MPEG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------