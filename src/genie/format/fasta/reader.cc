/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/reader.h"

#include <algorithm>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"
#include "genie/util/string_helpers.h"

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

// -----------------------------------------------------------------------------

FastaReader::FastaReader(std::istream& fasta_file, std::istream& fai_file,
                         std::istream& sha256_file, std::string path)
    : hash_file_(sha256_file),
      fai_(fai_file),
      fasta_(&fasta_file),
      path_(std::move(path)) {}

// -----------------------------------------------------------------------------

std::map<size_t, std::string> FastaReader::GetSequences() const {
  return fai_.GetSequences();
}

// -----------------------------------------------------------------------------

uint64_t FastaReader::GetLength(const std::string& name) const {
  return fai_.GetLength(name);
}

// -----------------------------------------------------------------------------

std::string FastaReader::LoadSection(const std::string& sequence,
                                     uint64_t start, const uint64_t end) const {
  const auto start_pos = fai_.GetFilePosition(sequence, start);
  std::string ret;
  ret.reserve(end - start);
  fasta_->seekg(static_cast<std::streamoff>(start_pos));
  std::string buffer;
  while (getline(*fasta_, buffer)) {
    start += buffer.size();
    if (start < end) {
      ret += buffer;
    } else if (start >= end) {
      ret += buffer.substr(0, buffer.size() - (start - end));
      break;
    }
  }
  std::transform(ret.begin(), ret.end(), ret.begin(), [](const char x) -> char {
    return static_cast<char>(toupper(x));
  });
  return ret;
}

// -----------------------------------------------------------------------------

void FastaReader::index(std::istream& fasta, std::ostream& fai) {
  std::string buffer;
  FaiFile fai_file;
  FaiFile::FaiSequence seq;
  bool last_line = false;
  uint64_t next_offset = 0;
  while (getline(fasta, buffer)) {
    if (buffer.empty()) {
      continue;
    }
    if (buffer.front() == '>') {
      last_line = false;
      if (seq.offset != 0) {
        const uint64_t tmp = seq.offset;
        seq.offset = next_offset;
        fai_file.AddSequence(seq);
        seq.offset = tmp;
      }
      seq.name = buffer.substr(1, buffer.find_first_of(' ') - 1);
      seq.offset += buffer.size() + 1;
      next_offset = seq.offset;
      UTILS_DIE_IF(!getline(fasta, buffer), "Missing line in fasta");
      seq.length = 0;
      seq.line_bases = buffer.size();
      seq.line_width = buffer.size() + 1;
    } else {
      UTILS_DIE_IF(last_line, "Invalid fasta line length");
    }
    seq.length += buffer.size();
    seq.offset += buffer.size() + 1;
    if (buffer.size() != seq.line_bases) {
      last_line = true;
    }
  }
  seq.offset = next_offset;
  fai_file.AddSequence(seq);
  fai << fai_file;
}

// -----------------------------------------------------------------------------

void FastaReader::hash(const FaiFile& fai, std::istream& fasta,
                       std::ostream& hash) {
  std::vector<std::pair<std::string, std::string>> hashes;
  for (const auto& [fst, snd] : fai.GetSequences()) {
    const auto pos = fai.GetFilePosition(snd, 0);
    const auto length = fai.GetLength(snd);
    auto sha_value = Sha256File::hash(fasta, pos, length);
    hashes.emplace_back(snd, sha_value);
  }
  Sha256File::Write(hash, hashes);
}

// -----------------------------------------------------------------------------

core::meta::Reference FastaReader::GetMeta() const {
  const std::string basename =
      path_.substr(path_.find_last_of('/') + 1,
                   path_.find_last_of('.') - path_.find_last_of('/') - 1);
  auto f = std::make_unique<core::meta::external_ref::Fasta>(
      "file://" + path_, core::meta::ExternalRef::ChecksumAlgorithm::kSha256);

  size_t id = 0;
  auto* f_ptr = f.get();
  core::meta::Reference ret(basename, 0, 0, 0, std::move(f), "");
  for (const auto& [fst, snd] : hash_file_.GetData()) {
    ret.AddSequence(core::meta::Sequence(fst, fai_.GetLength(fst),
                                         static_cast<uint16_t>(id++)));
    f_ptr->AddChecksum(util::FromHex(snd));
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
