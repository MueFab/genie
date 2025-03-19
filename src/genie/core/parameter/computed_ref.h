/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_
#define SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <optional>

#include "genie/core/parameter/computed_ref_extended.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

/**
 * @brief
 */
class ComputedRef final {
 public:
  /**
   * @brief
   */
  enum class Algorithm : uint8_t {
    kReserved = 0,
    kRefTransform = 1,
    kPushIn = 2,
    kLocalAssembly = 3,
    kGlobalAssembly = 4
  };

 private:
  Algorithm cr_alg_id_;                           //!< @brief
  std::optional<ComputedRefExtended> extension_;  //!< @brief

 public:
  /**
   * @brief
   * @param cr
   * @return
   */
  bool operator==(const ComputedRef& cr) const;

  /**
   *
   */
  ComputedRef(ComputedRef&&) = default;

  /**
   *
   */
  ComputedRef(const ComputedRef&) = default;

  /**
   *
   */
  ComputedRef& operator=(ComputedRef&&) = default;

  /**
   *
   */
  ComputedRef& operator=(const ComputedRef&) = default;

  /**
   * @brief
   * @param cr_alg_id
   */
  explicit ComputedRef(Algorithm cr_alg_id);

  /**
   * @brief
   * @param reader
   */
  explicit ComputedRef(util::BitReader& reader);

  /**
   * @brief
   */
  ~ComputedRef() = default;

  /**
   * @brief
   * @param computed_reference
   */
  void SetExtension(ComputedRefExtended&& computed_reference);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const ComputedRefExtended& GetExtension() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] Algorithm GetAlgorithm() const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------