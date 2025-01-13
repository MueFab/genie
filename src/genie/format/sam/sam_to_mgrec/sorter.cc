/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/sam/sam_to_mgrec/sorter.h"

#include <iostream>
#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------

SubfileReader::SubfileReader(const std::string& fpath)
    : reader_(fpath, std::ios::binary), bitreader_(reader_), path_(fpath) {
  UTILS_DIE_IF(!reader_, "Cannot open file to read: " + fpath);
  if (good()) {
    rec_ = core::record::Record(bitreader_);
  }
}

// -----------------------------------------------------------------------------

SubfileReader::~SubfileReader() {
  bitreader_.FlushHeldBits();
  reader_.close();
}

// -----------------------------------------------------------------------------

core::record::Record SubfileReader::MoveRecord() {
  UTILS_DIE_IF(!rec_, "No record available.");
  auto ret = std::move(rec_.value());
  if (good()) {
    rec_ = core::record::Record(bitreader_);
  } else {
    rec_.reset();
  }
  return ret;
}

// -----------------------------------------------------------------------------

const std::optional<core::record::Record>& SubfileReader::GetRecord() const {
  return rec_;
}

// -----------------------------------------------------------------------------

bool SubfileReader::good() { return reader_.good() && reader_.peek() != EOF; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
