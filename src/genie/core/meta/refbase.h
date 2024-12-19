/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REFBASE_H_
#define SRC_GENIE_CORE_META_REFBASE_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>

#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Base class for reference information (internal and external)
 */
class RefBase {
 protected:
  /**
   * @brief Do not allow construction of interface
   */
  RefBase() = default;

 public:
  /**
   * @brief Reference types supported by the standard
   */
  enum class ReferenceType : uint8_t {
    kMpeggRef = 0,
    kRawRef = 1,
    kFastaRef = 2
  };

  /**
   * @brief Virtual destructor to allow inheritance
   */
  virtual ~RefBase() = default;

  /**
   * @brief Convert to json
   * @return Json representation
   */
  [[nodiscard]] virtual nlohmann::json ToJson() const = 0;

  /**
   * @brief Get Json Key of this base (internal vs external ref)
   * @return Json key
   */
  [[nodiscard]] virtual const std::string& GetKeyName() const = 0;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual std::unique_ptr<RefBase> Clone() const = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REFBASE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
