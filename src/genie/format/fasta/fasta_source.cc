/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/fasta_source.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "genie/util/log.h"
#include "genie/util/ordered_section.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Fasta";

namespace genie::format::fasta {

// -----------------------------------------------------------------------------

FastaSource::FastaSource(std::ostream* outfile, core::ReferenceManager* ref_mgr)
    : outfile_(outfile), ref_mgr_(ref_mgr), line_length_(0) {
  auto seqs = ref_mgr_->GetSequences();
  size_t pos = 0;
  for (const auto& s : seqs) {
    pos +=
        (ref_mgr_->GetLength(s) - 1) / core::ReferenceManager::GetChunkSize() +
        1;
    cumulative_reference_lengths_[s] = pos;
  }
}

// -----------------------------------------------------------------------------

bool FastaSource::Pump(uint64_t& id, std::mutex& lock) {
  util::Section loc_id = {0, 1, false};
  {
    std::lock_guard guard(lock);
    loc_id.start = id;
    id++;
  }

  if (loc_id.start >= cumulative_reference_lengths_.rbegin()->second) {
    return false;
  }
  std::string seq;
  size_t pos_old = 0;
  size_t pos = 0;
  for (const auto& [fst, snd] : cumulative_reference_lengths_) {
    if (loc_id.start < snd) {
      pos = loc_id.start - pos_old;
      seq = fst;
      break;
    }
    pos_old = snd;
  }

  const auto string =
      ref_mgr_->LoadAt(seq, pos * core::ReferenceManager::GetChunkSize());
  const size_t actual_length =
      loc_id.start == cumulative_reference_lengths_[seq] - 1
          ? ref_mgr_->GetLength(seq) % core::ReferenceManager::GetChunkSize()
          : string->length();

  UTILS_LOG(util::Logger::Severity::INFO,
            "Decompressing " + seq + " [" +
                std::to_string(pos * core::ReferenceManager::GetChunkSize()) +
                ", " +
                std::to_string(pos * core::ReferenceManager::GetChunkSize() +
                               actual_length) +
                "]");

  [[maybe_unused]] util::OrderedSection out_sec(&output_lock_, loc_id);
  if (pos == 0) {
    if (line_length_ > 0) {
      outfile_->write("\n", 1);
      line_length_ = 0;
    }
    outfile_->write(">", 1);
    outfile_->write(seq.c_str(), static_cast<std::streamsize>(seq.length()));
    outfile_->write("\n", 1);
  }
  size_t s_pos = 0;
  while (true) {
    const size_t length = std::min(50 - line_length_, actual_length - s_pos);
    outfile_->write(&(*string)[s_pos], static_cast<std::streamsize>(length));
    s_pos += length;
    line_length_ += length;
    if (line_length_ == 50) {
      outfile_->write("\n", 1);
      line_length_ = 0;
    }
    if (s_pos == actual_length) {
      return true;
    }
  }
}

// -----------------------------------------------------------------------------

void FastaSource::FlushIn(uint64_t&) {}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------
