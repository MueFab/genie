/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/manager.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/format/fasta/reference.h"

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

// -----------------------------------------------------------------------------

Manager::Manager(std::istream& fasta, std::istream& fai, std::istream& sha,
                 core::ReferenceManager* mgr, std::string path)
    : ReferenceSource(mgr), reader_(fasta, fai, sha, std::move(path)) {
  auto ref = GenerateRefHandles();
  int i = 0;
  for (auto& r : ref) {
    mgr->AddRef(i++, std::move(r));
  }
}

// -----------------------------------------------------------------------------

std::map<size_t, std::string> Manager::GetSequences() const {
  return reader_.GetSequences();
}

// -----------------------------------------------------------------------------

uint64_t Manager::GetLength(const std::string& seq) const {
  return reader_.GetLength(seq);
}

// -----------------------------------------------------------------------------

std::string Manager::GetRef(const std::string& sequence, const uint64_t start,
                            const uint64_t end) {
  std::lock_guard guard(reader_mutex_);
  return reader_.LoadSection(sequence, start, end);
}

// -----------------------------------------------------------------------------

std::vector<std::unique_ptr<core::Reference>> Manager::GenerateRefHandles() {
  const auto seqs = GetSequences();
  std::vector<std::unique_ptr<core::Reference>> ret;
  for (const auto& [fst, snd] : seqs) {
    size_t length = GetLength(snd);
    ret.emplace_back(std::make_unique<Reference>(snd, length, this));
  }
  return ret;
}

// -----------------------------------------------------------------------------

core::meta::Reference Manager::GetMeta() const { return reader_.GetMeta(); }

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
