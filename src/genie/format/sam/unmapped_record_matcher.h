/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_UNMAPPED_RECORD_MATCHER_H_
#define SRC_GENIE_FORMAT_SAM_UNMAPPED_RECORD_MATCHER_H_

#include <algorithm>
#include <fstream>
#include <utility>

#include "genie/format/sam/sam_record.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

class UnmappedRecordMatcher {
  std::optional<SamRecord> last_record_;

 public:
  [[nodiscard]] std::optional<SamRecordPair> AddSamRead(
      const SamRecord& record) {
    constexpr auto kLogModuleName = "SamImporter";
    if (!last_record_) {
      last_record_ = record;
      return std::nullopt;
    }
    if (last_record_->qname_ == record.qname_) {
      auto ret = std::pair(std::move(*last_record_), record);
      last_record_.reset();
      return ret;
    }
    UTILS_LOG(util::Logger::Severity::WARNING,
              "Unmapped, unsorted record dropped");
    last_record_ = record;
    return std::nullopt;
  }

  void finish() {
    // TODO(fabian): Implement
  }

  std::optional<SamRecordPair> GetAfterFinish() {
    if (last_record_) {
      auto ret = std::pair(std::move(*last_record_), std::nullopt);
      last_record_.reset();
      return ret;
    }
    // TODO(fabian): Implement
    return std::nullopt;
  }
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_FORMAT_SAM_UNMAPPED_RECORD_MATCHER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
