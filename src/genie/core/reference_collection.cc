/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/reference_collection.h"

#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
std::string ReferenceCollection::GetSequence(const std::string& name,
                                             const uint64_t start,
                                             const uint64_t end) const {
  const auto it = refs_.find(name);
  if (it == refs_.end()) {
    return "";
  }
  uint64_t position = start;
  std::string ret;
  ret.reserve(end - start);
  while (true) {
    size_t nearest = std::numeric_limits<size_t>::max();
    for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
      if (position >= end) {
        return ret;
      }

      if (it2->get()->GetStart() > position) {
        nearest = std::min<size_t>(nearest, it2->get()->GetStart());
      }

      if (it2->get()->GetStart() <= position &&
          it2->get()->GetEnd() > position) {
        nearest = std::numeric_limits<size_t>::max();
        ret += it2->get()->GetSequence(
            position, std::min<uint64_t>(it2->get()->GetEnd(), end));
        position = it2->get()->GetEnd();
        it2 = it->second.begin();
      }
    }

    if (position >= end) {
      return ret;
    }

    if (nearest == std::numeric_limits<size_t>::max()) {
      ret += std::string(end - position, 'N');
      return ret;
    }
    ret += std::string(nearest - position, 'N');
    position = nearest;
  }
}

// -----------------------------------------------------------------------------
void ReferenceCollection::RegisterRef(std::unique_ptr<Reference> ref) {
  refs_[ref->GetName()].emplace_back(std::move(ref));
}

// -----------------------------------------------------------------------------
void ReferenceCollection::RegisterRef(
    std::vector<std::unique_ptr<Reference>>&& ref) {
  auto vec = std::move(ref);
  for (auto& v : vec) {
    RegisterRef(std::move(v));
  }
}

// -----------------------------------------------------------------------------
std::vector<std::pair<size_t, size_t>> ReferenceCollection::GetCoverage(
    const std::string& name) const {
  std::vector<std::pair<size_t, size_t>> ret;
  const auto it = refs_.find(name);
  if (it == refs_.end()) {
    return ret;
  }
  for (const auto& s : it->second) {
    ret.emplace_back(s->GetStart(), s->GetEnd());
  }
  return ret;
}

// -----------------------------------------------------------------------------
std::vector<std::string> ReferenceCollection::GetSequences() const {
  std::vector<std::string> ret;
  ret.reserve(refs_.size());
  for (const auto& [kFst, kSnd] : refs_) {
    ret.emplace_back(kFst);
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
