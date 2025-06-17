/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_DATASET_GROUP_H_
#define SRC_GENIE_CORE_META_DATASET_GROUP_H_

// -----------------------------------------------------------------------------

#include <string>

#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Information about a dataset group
 */
class DatasetGroup {
  uint16_t dataset_group_id_;  //!< @brief ID of dataset group
  uint8_t version_number_;     //!< @brief Current iteration, counting up for
                               //!< every change
  std::string dg_metadata_value_;  //!< @brief MPEG-G part 3 meta information
  std::string
      dg_protection_value_;  //!< @brief MPEG-G part 3 protection information

 public:
  /**
   * @brief
   * @param meta
   */
  void SetMetadata(std::string meta);

  /**
   * @brief
   * @param prot
   */
  void SetProtection(std::string prot);

  /**
   * @brief Return dataset group ID
   * @return dataset group ID
   */
  [[nodiscard]] uint16_t GetId() const;

  /**
   * @brief Return current iteration version
   * @return Current version
   */
  [[nodiscard]] uint8_t GetVersion() const;

  /**
   * @brief Return MPEG-G part 3 meta information
   * @return MPEG-G part 3 meta information
   */
  [[nodiscard]] const std::string& GetInformation() const;

  /**
   * @brief
   * @return
   */
  std::string& GetInformation();

  /**
   * @brief
   * @return
   */
  std::string& GetProtection();

  /**
   * @brief Return MPEG-G part 3 meta information
   * @return MPEG-G part 3 protection information
   */
  [[nodiscard]] const std::string& GetProtection() const;

  /**
   * @brief Construct from raw values
   * @param id ID of dataset group
   * @param version Current version number
   * @param meta MPEG-G part 3 meta information
   * @param protection MPEG-G part 3 protection information
   */
  DatasetGroup(uint16_t id, uint8_t version, std::string meta,
               std::string protection);

  /**
   * @brief Construct from json
   * @param json Json representation
   */
  explicit DatasetGroup(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DATASET_GROUP_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
