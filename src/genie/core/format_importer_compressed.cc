/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/format_importer_compressed.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

void FormatImporterCompressed::FlushIn(uint64_t& pos) { FlushOut(pos); }

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
