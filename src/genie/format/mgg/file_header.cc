/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/file_header.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

const std::string& FileHeader::GetMajorBrand() const { return major_brand_; }

// -----------------------------------------------------------------------------

core::MpegMinorVersion FileHeader::GetMinorVersion() const {
  return minor_version_;
}

// -----------------------------------------------------------------------------

void FileHeader::AddCompatibleBrand(std::string brand) {
  compatible_brands_.emplace_back(std::move(brand));
}

// -----------------------------------------------------------------------------

const std::vector<std::string>& FileHeader::GetCompatibleBrands() const {
  return compatible_brands_;
}

// -----------------------------------------------------------------------------

const std::string& FileHeader::GetKey() const {
  static const std::string key = "flhd";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------

FileHeader::FileHeader()
    : major_brand_("MPEG-G"), minor_version_(core::MpegMinorVersion::kV2000) {}

// -----------------------------------------------------------------------------

FileHeader::FileHeader(const core::MpegMinorVersion minor_version)
    : major_brand_("MPEG-G"), minor_version_(minor_version) {}

// -----------------------------------------------------------------------------

FileHeader::FileHeader(util::BitReader& bit_reader)
    : major_brand_(6, '\0'), minor_version_() {
  const auto start_pos = bit_reader.GetStreamPosition() - 4;
  const auto length = bit_reader.ReadAlignedInt<uint64_t>();
  const auto num_compatible_brands = (length - 22) / 4;
  bit_reader.ReadAlignedBytes(major_brand_.data(), major_brand_.length());
  UTILS_DIE_IF(major_brand_ != "MPEG-G", "Not an MPEG-G file");
  std::string tmp(4, '\0');
  bit_reader.ReadAlignedBytes(tmp.data(), tmp.length());
  minor_version_ = core::GetMpegVersion(tmp);
  compatible_brands_.resize(num_compatible_brands, std::string(4, '\0'));
  for (auto& b : compatible_brands_) {
    bit_reader.ReadAlignedBytes(b.data(), b.length());
  }
  UTILS_DIE_IF(start_pos + length != bit_reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

void FileHeader::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteAlignedBytes(major_brand_.data(), major_brand_.length());
  const auto tmp = GetMpegVersionString(minor_version_);
  bit_writer.WriteAlignedBytes(tmp.data(), tmp.length());
  for (auto& b : compatible_brands_) {
    bit_writer.WriteAlignedBytes(b.data(), b.length());
  }
}

// -----------------------------------------------------------------------------

bool FileHeader::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const FileHeader&>(info);
  return major_brand_ == other.major_brand_ &&
         minor_version_ == other.minor_version_ &&
         compatible_brands_ == other.compatible_brands_;
}

// -----------------------------------------------------------------------------

void FileHeader::PrintDebug(std::ostream& output, const uint8_t depth,
                             const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* File Header");
  print_offset(output, depth + 1, max_depth, "Major brand: " + major_brand_);
  print_offset(output, depth + 1, max_depth,
               "Minor version: " + GetMpegVersionString(minor_version_));
  for (const auto& b : compatible_brands_) {
    print_offset(output, depth + 1, max_depth, "Compatible brand: " + b);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
