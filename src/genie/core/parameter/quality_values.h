/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_QUALITY_VALUES_H_
#define SRC_GENIE_CORE_PARAMETER_QUALITY_VALUES_H_

// -----------------------------------------------------------------------------

#include <memory>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

/**
 * @brief
 */
class QualityValues {
 public:
  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const = 0;

  /**
   * @brief
   */
  virtual ~QualityValues() = default;

  /**
   * @brief
   * @param qv_coding_mode
   * @param qv_reverse_flag
   */
  QualityValues(uint8_t qv_coding_mode, bool qv_reverse_flag);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual std::unique_ptr<QualityValues> Clone() const = 0;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetMode() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual size_t GetNumSubsequences() const = 0;

  /**
   * @brief
   * @param qv
   * @return
   */
  virtual bool Equals(const QualityValues* qv) const;

 protected:
  uint8_t qv_coding_mode_;  //!< @brief
  bool qv_reverse_flag_;    //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_QUALITY_VALUES_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------