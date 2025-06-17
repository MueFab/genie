/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#ifndef SRC_GENIE_CORE_META_BLOCK_HEADER_H_
#define SRC_GENIE_CORE_META_BLOCK_HEADER_H_

// -----------------------------------------------------------------------------

#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Block header presence interface
 */
class BlockHeader {
 public:
  /**
   * @brief Block header modes
   */
  enum class HeaderType : uint8_t { kDisabled = 0, kEnabled = 1 };

 private:
  HeaderType type_;  //!< @brief Active type

 protected:
  /**
   * @brief Construct from type
   * @param type Block header type
   */
  explicit BlockHeader(HeaderType type);

 public:
  /**
   * @brief Virtual destructor for inheritance
   */
  virtual ~BlockHeader() = default;

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] virtual nlohmann::json ToJson() const = 0;

  /**
   * @brief Return block header mode
   * @return Block header mode
   */
  [[nodiscard]] HeaderType GetType() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_BLOCK_HEADER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------