/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_CORE_META_DESCRIPTOR_STREAM_H_

// -----------------------------------------------------------------------------

#include <string>

#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Meta information for descriptor stream
 */
class DescriptorStream {
  size_t descriptor_id_;             //!< @brief ID of descriptor stream
  std::string ds_protection_value_;  //!< @brief Part 3 protection information

 public:
  /**
   * @brief Construct from raw values
   * @param descriptor_id Descriptor ID
   * @param ds_protection_value Part 3 protection information
   */
  DescriptorStream(size_t descriptor_id, std::string ds_protection_value);

  /**
   * @brief Construct from json
   * @param obj Json representation
   */
  explicit DescriptorStream(const nlohmann::json& obj);

  /**
   * @brief Get descriptor ID
   * @return Descriptor ID
   */
  [[nodiscard]] size_t GetId() const;

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Return Part 3 protection information
   * @return Part 3 protection information
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

#endif  // SRC_GENIE_CORE_META_DESCRIPTOR_STREAM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
