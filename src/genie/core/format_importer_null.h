/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FORMAT_IMPORTER_NULL_H_
#define SRC_GENIE_CORE_FORMAT_IMPORTER_NULL_H_

// -----------------------------------------------------------------------------

#include "genie/core/format_importer.h"

// -----------------------------------------------------------------------------

namespace genie::core {
/**
 * @brief
 */
class NullImporter final : public FormatImporter {
 protected:
  /**
   * @brief
   * @param classifier
   * @return
   */
  bool PumpRetrieve(Classifier* classifier) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_IMPORTER_NULL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
