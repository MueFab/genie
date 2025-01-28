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

#include "genie/format/sam/sam_to_mgrec/sam_reader.h"
#include "genie/format/sam/sam_record.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------

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

int SamReader::ReadSamQuery(std::vector<SamRecord>& sr) {
  sr.clear();
  if (buffered_rec_) {
    sr.push_back(std::move(buffered_rec_.value()));
    buffered_rec_.reset();
  } else {
    if (const auto res = sam_read1(sam_file_, sam_header, sam_alignment_);
        res >= 0) {
      sr.emplace_back(sam_alignment_);
    } else {
      return res;
    }
  }

  while (true) {
    if (const auto res = sam_read1(sam_file_, sam_header, sam_alignment_);
        res >= 0) {
      buffered_rec_ = SamRecord(sam_alignment_);
    } else {
      return res;
    }
    if (buffered_rec_->qname_ != sr.front().qname_) {
      return 0;
    }
    sr.push_back(std::move(buffered_rec_.value()));
    buffered_rec_.reset();
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
