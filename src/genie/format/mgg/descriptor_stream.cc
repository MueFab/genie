/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/descriptor_stream.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

bool DescriptorStream::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DescriptorStream&>(info);
  return header_ == other.header_ && ds_protection_ == other.ds_protection_ &&
         payload_ == other.payload_;
}

// -----------------------------------------------------------------------------

DescriptorStream::DescriptorStream(DescriptorStreamHeader header)
    : header_(std::move(header)) {}

// -----------------------------------------------------------------------------

DescriptorStream::DescriptorStream(
    util::BitReader& reader, const MasterIndexTable& table,
    const std::vector<dataset_header::MitClassConfig>& configs) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.Read<uint64_t>();
  std::string tmp(4, '\0');
  reader.ReadAlignedBytes(tmp.data(), tmp.length());
  UTILS_DIE_IF(tmp != "dshd", "Descriptor stream without header");
  header_ = DescriptorStreamHeader(reader);

  reader.ReadAlignedBytes(tmp.data(), tmp.length());
  if (tmp == "dspr") {
    ds_protection_ = DescriptorStreamProtection(reader);
  } else {
    reader.SetStreamPosition(reader.GetStreamPosition() - 4);
  }

  uint8_t class_index = 0;
  uint8_t descriptor_index = 0;
  bool found = false;
  for (uint8_t i = 0; i < static_cast<uint8_t>(configs.size()); ++i) {
    if (configs[i].GetClassId() == header_.GetClassType()) {
      class_index = i;
      found = true;
      bool found2 = false;
      for (uint8_t j = 0;
           j < static_cast<uint8_t>(configs[i].GetDescriptorIDs().size());
           ++j) {
        if (configs[i].GetDescriptorIDs()[j] == header_.GetDescriptorId()) {
          descriptor_index = j;
          found2 = true;
          break;
        }
      }
      UTILS_DIE_IF(!found2, "Did not find descriptor config");
      break;
    }
  }
  UTILS_DIE_IF(!found, "Did not find class config");

  auto payload_sizes = table.GetDescriptorStreamOffsets(
      class_index, descriptor_index,
      header_.GetClassType() == core::record::ClassType::kClassU,
      length - (reader.GetStreamPosition() - start_pos));
  UTILS_DIE_IF(payload_sizes.size() < header_.GetNumBlocks(),
               "DS payload sizes not available");
  for (size_t i = 0; i < header_.GetNumBlocks(); ++i) {
    payload_.emplace_back(reader, payload_sizes[i]);
  }

  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

const DescriptorStreamHeader& DescriptorStream::GetHeader() const {
  return header_;
}

// -----------------------------------------------------------------------------

void DescriptorStream::AddPayload(core::Payload p) {
  payload_.emplace_back(std::move(p));
  header_.AddBlock();
}

// -----------------------------------------------------------------------------

const std::vector<core::Payload>& DescriptorStream::GetPayloads() const {
  return payload_;
}

// -----------------------------------------------------------------------------

void DescriptorStream::SetProtection(DescriptorStreamProtection protection) {
  ds_protection_ = std::move(protection);
}

// -----------------------------------------------------------------------------

const DescriptorStreamProtection& DescriptorStream::GetProtection() const {
  return *ds_protection_;
}

// -----------------------------------------------------------------------------

bool DescriptorStream::HasProtection() const {
  return ds_protection_ != std::nullopt;
}

// -----------------------------------------------------------------------------

void DescriptorStream::BoxWrite(util::BitWriter& writer) const {
  header_.Write(writer);
  if (ds_protection_ != std::nullopt) {
    ds_protection_->Write(writer);
  }
  for (const auto& p : payload_) {
    p.Write(writer);
  }
}

// -----------------------------------------------------------------------------

const std::string& DescriptorStream::GetKey() const {
  static const std::string key = "dscn";
  return key;
}

// -----------------------------------------------------------------------------

DescriptorStream::DescriptorStream(const core::GenDesc descriptor,
                                   const core::record::ClassType class_id,
                                   std::vector<mgb::Block>&& blocks)
    : header_(false, descriptor, class_id,
              static_cast<uint32_t>(blocks.size())) {
  for (auto& b : blocks) {
    payload_.emplace_back(b.MovePayloadUnparsed());
  }
}

// -----------------------------------------------------------------------------

std::vector<mgb::Block> DescriptorStream::Decapsulate() {
  std::vector<mgb::Block> ret;
  ret.reserve(payload_.size());
  for (auto& p : payload_) {
    ret.emplace_back(header_.GetDescriptorId(), std::move(p));
  }
  return ret;
}

// -----------------------------------------------------------------------------

bool DescriptorStream::IsEmpty() const {
  if (payload_.empty()) {
    return true;
  }

  return std::all_of(payload_.begin(), payload_.end(),
                     [](const auto& p) { return p.GetPayloadSize() == 0; });
}

// -----------------------------------------------------------------------------

DescriptorStreamProtection& DescriptorStream::GetProtection() {
  return *ds_protection_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
