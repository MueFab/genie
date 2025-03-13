/**
* Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_BLOCK_HEADER_DISABLED_H_
#define SRC_GENIE_CORE_META_BLOCK_HEADER_DISABLED_H_

// -----------------------------------------------------------------------------

#include "genie/core/meta/block_header.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta::block_header {

/**
 * @brief Represents block continuity without block headers
 */
class Disabled final : public BlockHeader {
 bool ordered_blocks_flag_;  //!< @brief True: Blocks are ordered by mapping
//!< position

public:
 /**
  * @brief Constructor from raw values
  * @param ordered ordered_blocks_flag
  */
 explicit Disabled(bool ordered);

 /**
  * @brief Constructor from json
  * @param json Json input
  */
 explicit Disabled(const nlohmann::json& json);

 /**
  * @brief Return ordered_blocks_flag
  * @return ordered_blocks_flag
  */
 [[nodiscard]] bool GetOrderedFlag() const;

 /**
  * @brief Convert to json
  * @return Json representation
  */
 [[nodiscard]] nlohmann::json ToJson() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::block_header

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCK_HEADER_DISABLED_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------