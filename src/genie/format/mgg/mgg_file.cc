/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/mgg_file.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/format/mgg/file_header.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Mgg";

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
MggFile::MggFile(std::istream* file) : file_(file), reader_(*file_) {
  while (true) {
    std::string boxname(4, '\0');
    reader_->ReadAlignedBytes(boxname.data(), boxname.length());
    if (!reader_->IsStreamGood()) {
      break;
    }
    UTILS_DIE_IF(boxes_.empty() && boxname != "flhd", "No file header found");
    UTILS_DIE_IF(!boxes_.empty() && boxname == "flhd",
                 "Multiple file headers found");
    if (boxname == "flhd") {
      boxes_.emplace_back(std::make_unique<FileHeader>(*reader_));
    } else if (boxname == "dgcn") {
      const auto& hdr = dynamic_cast<const FileHeader&>(*boxes_.front());
      boxes_.emplace_back(
          std::make_unique<DatasetGroup>(*reader_, hdr.GetMinorVersion()));
    } else {
      GENIE_LOG(util::Logger::Severity::WARNING,
                "Unknown Box " + boxname + " on top level of file. Skip.");
      break;
    }
  }
  file_->clear();
  file_->seekg(0, std::ios::beg);
}

// -----------------------------------------------------------------------------
std::vector<std::unique_ptr<Box>>& MggFile::GetBoxes() { return boxes_; }

// -----------------------------------------------------------------------------
MggFile::MggFile() : file_(nullptr) {}

// -----------------------------------------------------------------------------
void MggFile::AddBox(std::unique_ptr<Box> box) {
  boxes_.emplace_back(std::move(box));
}

// -----------------------------------------------------------------------------
void MggFile::Write(util::BitWriter& writer) const {
  for (const auto& b : boxes_) {
    b->Write(writer);
  }
}

// -----------------------------------------------------------------------------
void MggFile::print_debug(std::ostream& output, const uint8_t max_depth) const {
  for (const auto& b : boxes_) {
    b->PrintDebug(output, 0, max_depth);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
