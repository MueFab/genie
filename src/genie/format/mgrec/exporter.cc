/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgrec/exporter.h"

#include <utility>

#include "genie/util/ordered_section.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgrec {

// -----------------------------------------------------------------------------
Exporter::Exporter(std::ostream& file_1) : writer_(file_1) {}

// -----------------------------------------------------------------------------
void Exporter::FlowIn(core::record::Chunk&& t, const util::Section& id) {
  core::record::Chunk data = std::move(t);
  [[maybe_unused]] util::OrderedSection section(&lock_, id);
  GetStats().Add(data.GetStats());
  for (auto& i : data.GetData()) {
    i.Write(writer_);
  }
}

void Exporter::SkipIn(const util::Section& id) {
  [[maybe_unused]] util::OrderedSection sec(&lock_, id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
