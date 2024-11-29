/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_H_
#define SRC_GENIE_CORE_REFERENCE_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class Reference {
  std::string name_;  //!< @brief
  uint64_t start_;    //!< @brief
  uint64_t end_;      //!< @brief

 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetName() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetStart() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetEnd() const;

  /**
   * @brief
   * @param start
   * @param end
   * @return
   */
  virtual std::string GetSequence(uint64_t start, uint64_t end) = 0;

  /**
   * @brief
   */
  virtual ~Reference() = default;

  /**
   * @brief
   * @param name
   * @param start
   * @param end
   */
  Reference(std::string name, uint64_t start, uint64_t end);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
