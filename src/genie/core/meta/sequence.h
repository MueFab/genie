/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_SEQUENCE_H_
#define SRC_GENIE_CORE_META_SEQUENCE_H_

// -----------------------------------------------------------------------------

#include <string>

#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Reference sequence information
 */
class Sequence {
  /// Sequence name
  std::string name_;

  /// Sequence length
  uint64_t length_;

  /// Sequence ID
  uint16_t id_;

 public:
  /**
   * @brief
   * @return
   */
  std::string& GetName();

  /**
   * @brief Construct from raw values
   * @param name Sequence name
   * @param length  Sequence length
   * @param id Sequence id
   */
  explicit Sequence(std::string name, uint64_t length, uint16_t id);

  /**
   * @brief Construct from json
   * @param json Json representation
   */
  explicit Sequence(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Get Name of Sequence
   * @return Name
   */
  [[nodiscard]] const std::string& GetName() const;

  /**
   * @brief Get length of sequence
   * @return Length
   */
  [[nodiscard]] uint64_t GetLength() const;

  /**
   * @brief Get Sequence ID
   * @return ID
   */
  [[nodiscard]] uint16_t GetId() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_SEQUENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------