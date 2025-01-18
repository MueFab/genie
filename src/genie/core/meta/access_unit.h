/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_ACCESS_UNIT_H_
#define SRC_GENIE_CORE_META_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------

#include <string>

#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Holds metadata associated with an access unit
 */
class AccessUnit {
  size_t access_unit_id_;             //!< @brief ID of associated access unit
  std::string au_information_value_;  //!< @brief MPEG-G Part 3 metadata
  std::string au_protection_value_;   //!< @brief MPEG-G Part 3 protection data

 public:
  /**
   * @brief Construct from raw data
   * @param id ID of associated access unit
   * @param information MPEG-G Part 3 metadata
   * @param protection MPEG-G Part 3 protection data
   */
  AccessUnit(size_t id, std::string information, std::string protection);

  /**
   * @brief
   * @param id
   */
  explicit AccessUnit(size_t id);

  /**
   * @brief
   * @param information
   */
  void SetInformation(std::string information);

  /**
   * @brief
   * @param protection
   */
  void SetProtection(std::string protection);

  /**
   * @brief Construct form json
   * @param obj Json representation
   */
  explicit AccessUnit(const nlohmann::json& obj);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Get ID of associated access unit
   * @return AU ID
   */
  [[nodiscard]] size_t GetId() const;

  /**
   * @brief Get MPEG-G Part 3 metadata
   * @return MPEG-G Part 3 metadata
   */
  [[nodiscard]] const std::string& GetInformation() const;

  /**
   * @brief
   * @return
   */
  std::string& GetInformation();

  /**
   * @brief MPEG-G Part 3 protection data
   * @return MPEG-G Part 3 protection data
   */
  [[nodiscard]] const std::string& GetProtection() const;

  /**
   * @brief
   * @return
   */
  std::string& GetProtection();
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
