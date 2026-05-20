/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_BLOCK_HEADER_ENABLED_H_
#define SRC_GENIE_CORE_META_BLOCK_HEADER_ENABLED_H_

// -----------------------------------------------------------------------------

#include "genie/core/meta/block_header.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta::block_header {

/**
 * @brief Represents block continuity with block headers
 */
class Enabled final : public BlockHeader {
  bool mit_flag_;      //!< @brief If true, master index table is present
  bool cc_mode_flag_;  //!< @brief If true, access units are ordered by class
                       //!< instead mapping position

 public:
  /**
   * @brief Construct from raw values
   * @param mit MIT_flag
   * @param cc CC_mode_flag
   */
  Enabled(bool mit, bool cc);

  /**
   * @brief Construct from json
   * @param json Json representation
   */
  explicit Enabled(const nlohmann::json& json);

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] nlohmann::json ToJson() const override;

  /**
   * @brief Return MIT_flag
   * @return MIT_flag
   */
  [[nodiscard]] bool GetMitFlag() const;

  /**
   * @brief Return CC_mode_flag
   * @return CC_mode_flag
   */
  [[nodiscard]] bool GetCcFlag() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::block_header

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCK_HEADER_ENABLED_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------