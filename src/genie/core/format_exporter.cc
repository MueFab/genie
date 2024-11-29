/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/format_exporter.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
stats::PerfStats& FormatExporter::GetStats() { return stats_; }

// -----------------------------------------------------------------------------
void FormatExporter::SkipIn(const util::Section&) {}

// -----------------------------------------------------------------------------
void FormatExporter::FlushIn(uint64_t&) {}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
