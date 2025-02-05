/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include <cstring>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

#include "genie/format/sam/sam_reader.h"
#include "genie/format/sam/sam_record.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

// -----------------------------------------------------------------------------

void SamReader::InternalRead() {
  if (sam_read1(sam_file_, sam_header, sam_alignment_) >= 0) {
    buffer_ = SamRecord(sam_alignment_);
  } else {
    buffer_.reset();
  }
}

SamReader::SamReader(const std::string& fpath)
    : sam_file_(nullptr),           // open bam file
      sam_header(nullptr),          // read header
      sam_alignment_(bam_init1()),  // initialize an alignment
      header_info(KS_INITIALIZE) {
  if (fpath.substr(0, 2) == "-.") {
    sam_file_ = hts_open("-", "r");
  } else {
    sam_file_ = hts_open(fpath.c_str(), "r");
  }
  UTILS_DIE_IF(!sam_file_, "Could not open file: " + fpath);
  sam_header = sam_hdr_read(sam_file_);
  UTILS_DIE_IF(!sam_header, "Could not read header from file: " + fpath);
  InternalRead();
}

// -----------------------------------------------------------------------------

SamReader::~SamReader() {
  bam_destroy1(sam_alignment_);
  bam_hdr_destroy(sam_header);
  if (sam_file_) sam_close(sam_file_);
  if (header_info.s) free(header_info.s);
}

// -----------------------------------------------------------------------------

std::vector<std::pair<std::string, size_t>> SamReader::GetRefs() const {
  std::vector<std::pair<std::string, size_t>> ret;
  const auto num_ref = sam_hdr_nref(sam_header);
  ret.reserve(num_ref);
  for (int i = 0; i < num_ref; ++i) {
    ret.emplace_back(sam_hdr_tid2name(sam_header, i),
                     sam_hdr_tid2len(sam_header, i));
  }
  return ret;
}

// -----------------------------------------------------------------------------

bool SamReader::IsReady() {
  if (!sam_file_) {
    return false;
  }

  sam_header = sam_hdr_read(sam_file_);  // read header
  if (!sam_header) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------

bool SamReader::IsValid() {
  /// Find out if records are sorted by query name
  UTILS_DIE_IF(sam_hdr_find_tag_hd(sam_header, "SO", &header_info) != 0 ||
                   std::strcmp(header_info.s, "queryname") != 0,
               "Sam file must be ordered by read name! That ordering must be "
               "documented in the SAM header.");

  return true;
}

// -----------------------------------------------------------------------------

void SamReader::Read() {
  if (buffer_) {
    InternalRead();
  }
}

// -----------------------------------------------------------------------------

const std::optional<SamRecord>& SamReader::Peek() const { return buffer_; }

std::optional<SamRecord> SamReader::Move() { return std::move(buffer_); }

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
