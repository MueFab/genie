/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/fai_file.h"

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

// -----------------------------------------------------------------------------

FaiFile::FaiSequence::FaiSequence(std::istream& stream) {
  const std::string error_msg = "Fai parsing failed";
  UTILS_DIE_IF(!std::getline(stream, name), error_msg);
  std::string buffer;
  UTILS_DIE_IF(!std::getline(stream, buffer), error_msg);
  length = std::stoll(buffer);
  UTILS_DIE_IF(!std::getline(stream, buffer), error_msg);
  offset = std::stoll(buffer);
  UTILS_DIE_IF(!std::getline(stream, buffer), error_msg);
  line_bases = std::stol(buffer);
  UTILS_DIE_IF(!std::getline(stream, buffer), error_msg);
  line_width = std::stol(buffer);
}

// -----------------------------------------------------------------------------

FaiFile::FaiSequence::FaiSequence()
    : length(0), offset(0), line_bases(0), line_width(0) {}

// -----------------------------------------------------------------------------

void FaiFile::AddSequence(const FaiSequence& seq) {
  seqs_.insert(std::make_pair(seq.name, seq));
  indices_.insert(std::make_pair(indices_.size(), seq.name));
}

// -----------------------------------------------------------------------------

FaiFile::FaiFile(std::istream& stream) {
  while (stream.peek() != EOF) {
    FaiSequence seq(stream);
    AddSequence(seq);
  }
}

// -----------------------------------------------------------------------------

uint64_t FaiFile::GetFilePosition(const std::string& sequence,
                                  const uint64_t position) const {
  const auto seq = seqs_.find(sequence);
  UTILS_DIE_IF(seq == seqs_.end(), "Unknown ref sequence");
  UTILS_DIE_IF(seq->second.length < position,
               "Reference position out of bounds");
  const uint64_t offset = seq->second.offset;
  const uint64_t full_base_lines = position / seq->second.line_bases;
  const uint64_t full_base_lines_bytes =
      full_base_lines * seq->second.line_width;
  const uint64_t last_line = position % seq->second.line_bases;
  const uint64_t pos = offset + full_base_lines_bytes + last_line;
  return pos;
}

// -----------------------------------------------------------------------------

std::map<size_t, std::string> FaiFile::GetSequences() const {
  std::map<size_t, std::string> ret;
  for (const auto& [fst, snd] : indices_) {
    ret.insert(std::make_pair(fst, snd));
  }
  return ret;
}

// -----------------------------------------------------------------------------

bool FaiFile::ContainsSequence(const std::string& seq) const {
  return seqs_.find(seq) != seqs_.end();
}

// -----------------------------------------------------------------------------

uint64_t FaiFile::GetLength(const std::string& seq) const {
  UTILS_DIE_IF(!ContainsSequence(seq), "Unknown ref sequence");
  return seqs_.at(seq).length;
}

// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream,
                         const FaiFile::FaiSequence& file) {
  stream << file.name << "\n";
  stream << file.length << "\n";
  stream << file.offset << "\n";
  stream << file.line_bases << "\n";
  stream << file.line_width << std::endl;
  return stream;
}

// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const FaiFile& file) {
  std::map<uint64_t, FaiFile::FaiSequence> sorted_map;
  for (const auto& [fst, snd] : file.seqs_) {
    sorted_map.insert(std::make_pair(snd.offset, snd));
  }
  for (const auto& [fst, snd] : sorted_map) {
    stream << snd;
  }
  return stream;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
