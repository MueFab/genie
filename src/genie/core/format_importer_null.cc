/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/format_importer_null.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

bool NullImporter::PumpRetrieve(Classifier*) { return false; }

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
