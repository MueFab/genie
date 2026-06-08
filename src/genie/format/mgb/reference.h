/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_REFERENCE_H_
#define SRC_GENIE_FORMAT_MGB_REFERENCE_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/reference.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

class Importer;

/**
 * @brief
 */
class Reference final : public core::Reference {
  ///
  Importer* mgr_;

  ///
  size_t f_pos_;

  ///
  bool raw_;

 public:
  /**
   *
   * @param name
   * @param start
   * @param end
   * @param mgr
   * @param filepos
   * @param raw
   */
  Reference(const std::string& name, const size_t start, const size_t end,
            Importer* mgr, const size_t filepos, const bool raw)
      : core::Reference(name, start, end),
        mgr_(mgr),
        f_pos_(filepos),
        raw_(raw) {}

  /**
   * @brief
   * @param start
   * @param end
   * @return
   */
  std::string GetSequence(uint64_t start, uint64_t end) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_REFERENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
