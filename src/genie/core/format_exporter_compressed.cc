/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/format_exporter_compressed.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

stats::PerfStats& FormatExporterCompressed::GetStats() { return stats_; }

// -----------------------------------------------------------------------------

void FormatExporterCompressed::SkipIn(const util::Section&) {}

// -----------------------------------------------------------------------------

void FormatExporterCompressed::FlushIn(uint64_t&) {}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
