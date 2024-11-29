/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/format_importer.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
void FormatImporter::SetClassifier(Classifier* classifier) {
  classifier_ = classifier;
}

// -----------------------------------------------------------------------------
bool FormatImporter::Pump(uint64_t& id, std::mutex& lock) {
  record::Chunk chunk;
  util::Section sec{};
  {
    std::unique_lock guard(lock);
    chunk = classifier_->GetChunk();
    uint32_t segment_count = 0;
    for (const auto& r : chunk.GetData()) {
      segment_count += static_cast<uint32_t>(r.GetSegments().size());
    }
    if (chunk.GetData().empty()) {
      segment_count = 1;
    }
    if (!chunk.GetData().empty() || !chunk.GetRefToWrite().empty()) {
      sec = {id, segment_count, true};
      id += segment_count;
    } else {
      const bool data_left = PumpRetrieve(classifier_);
      if (!data_left && !flushing_) {
        classifier_->Flush();
        flushing_ = true;
        return true;
      }
      if (!data_left && flushing_) {
        flushing_ = false;
        return false;
      }
    }
  }
  if (!chunk.GetData().empty() || !chunk.GetRefToWrite().empty()) {
    FlowOut(std::move(chunk), sec);
  }
  return true;
}

// -----------------------------------------------------------------------------
void FormatImporter::FlushIn(uint64_t& pos) { FlushOut(pos); }

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
