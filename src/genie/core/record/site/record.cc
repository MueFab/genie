/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/parameter/annotation/record.h"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "genie/core/array_type.h"
#include "genie/core/record/site/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// -------------------------------------------------------------------------------------------------

namespace genie::core::record::variant_site {

// -------------------------------------------------------------------------------------------------

Record::Record(util::BitReader& reader) {
  Read(reader);
}

// -------------------------------------------------------------------------------------------------

void Record::Write(genie::util::BitWriter& writer) {
  writer.WriteBits(variant_index_, 64);
  writer.WriteBits(seq_id_, 16);
  writer.WriteBits(pos_, 40);
  writer.WriteBits(strand_, 8);
  writer.WriteBits(id_len_, 8);
  writer.WriteAlignedBytes(id_.data(), id_.size());
  writer.WriteBits(description_len_, 8);
  writer.WriteAlignedBytes(description_.data(), description_.size());
  writer.WriteBits(ref_len_, 32);
  writer.WriteAlignedBytes(ref_.data(), ref_.size());

  writer.WriteBits(alt_count_, 8);
  for (auto idx_i = 0; idx_i < alt_count_; ++idx_i) {
    writer.WriteBits(alt_len_[idx_i], 32);
    for (char c : altern_[idx_i]) {
      writer.WriteBits(static_cast<uint8_t>(c), 8);
    }
  }
  writer.WriteBits(depth_, 32);
  writer.WriteBits(seq_qual_, 32);
  writer.WriteBits(map_qual_, 32);
  writer.WriteBits(map_num_qual_0_, 32);
  writer.WriteBits(filters_len_, 8);
  writer.WriteAlignedBytes(filters_.data(), filters_.size());

  auto info_tag = info_.GetFields();
  writer.WriteBits(static_cast<uint8_t>(info_tag.size()), 8);
  for (auto idx_i = 0u; idx_i < info_tag.size(); ++idx_i) {
    writer.WriteBits(info_tag[idx_i].name.size(), 8);
    writer.WriteAlignedBytes(info_tag[idx_i].name.data(), info_tag[idx_i].name.size());
    writer.WriteBits(static_cast<uint8_t>(info_tag[idx_i].type), 8);
    writer.WriteBits(info_tag[idx_i].values.size(), 8);
    ArrayType writeType;
    for (auto idx_j = 0u; idx_j < info_tag[idx_i].values.size(); ++idx_j) {
      writeType.toFile(info_tag[idx_i].type, info_tag.at(idx_i).values.at(idx_j), writer);
      if (info_tag[idx_i].type == DataType::STRING)
        writer.WriteBits(0, 8);
    }
  }
  writer.WriteBits(0, 7);
  writer.WriteBits(linked_record_, 1);
  if (linked_record_) {
    writer.WriteBits(link_name_len_, 8);
    writer.WriteAlignedBytes(link_name_.data(), link_name_.size());
    writer.WriteBits(reference_box_id_, 8);
  }
}

// -------------------------------------------------------------------------------------------------

bool Record::Read(genie::util::BitReader& reader) {
  ClearData();
  variant_index_ = reader.ReadBits(64);
  if (!reader.IsStreamGood())
    return false;
  seq_id_ = static_cast<uint16_t>(reader.ReadBits(16));
  pos_ = reader.ReadBits(40);
  strand_ = static_cast<uint8_t>(reader.ReadBits(8));
  id_len_ = static_cast<uint8_t>(reader.ReadBits(8));
  if (id_len_ > 0) {
    id_.resize(id_len_);
    reader.ReadAlignedBytes(&id_[0], id_len_);
  }
  description_len_ = static_cast<uint8_t>(reader.ReadBits(8));

  if (description_len_ > 0) {
    description_.resize(description_len_);
    reader.ReadAlignedBytes(&description_[0], description_len_);
  }
  ref_len_ = static_cast<uint32_t>(reader.ReadBits(32));
  if (ref_len_ > 0) {
    ref_.resize(ref_len_);
    reader.ReadAlignedBytes(&ref_[0], ref_len_);
  }
  alt_count_ = static_cast<uint8_t>(reader.ReadBits(8));
  for (auto idx_i = 0; idx_i < alt_count_; ++idx_i) {
    alt_len_.push_back(static_cast<uint32_t>(reader.ReadBits(32)));
    std::string altlist(alt_len_.back(), 0);
    for (auto& item : altlist)
      item = static_cast<char>(reader.ReadBits(8));
    altern_.push_back(altlist);
  }

  depth_ = static_cast<uint32_t>(reader.ReadBits(32));

  seq_qual_ = static_cast<uint32_t>(reader.ReadBits(32));
  map_qual_ = static_cast<uint32_t>(reader.ReadBits(32));
  map_num_qual_0_ = static_cast<uint32_t>(reader.ReadBits(32));

  filters_len_ = static_cast<uint8_t>(reader.ReadBits(8));
  if (filters_len_ > 0) {
    filters_.resize(filters_len_);
    reader.ReadAlignedBytes(&filters_[0], filters_len_);
  }

  info_.Read(reader);

  linked_record_ = static_cast<uint8_t>(reader.ReadBits(8));
  if (linked_record_) {
    link_name_len_ = static_cast<uint8_t>(reader.ReadBits(8));
    if (link_name_len_ > 0) {
      link_name_.resize(link_name_len_);
      reader.ReadAlignedBytes(&link_name_[0], link_name_len_);
    }
    reference_box_id_ = static_cast<uint8_t>(reader.ReadBits(8));
  }
  return true;
}

// -------------------------------------------------------------------------------------------------

void Record::ClearData() {
  variant_index_ = 0;
  seq_id_ = 0;
  pos_ = 0;
  strand_ = 0;
  id_len_ = 0;
  id_ = "";
  description_len_ = 0;
  description_ = "";
  ref_len_ = 0;
  ref_ = "";
  alt_count_ = 0;
  alt_len_ = {};
  altern_ = {};
  depth_ = 0;
  seq_qual_ = 0;
  map_qual_ = 0;
  map_num_qual_0_ = 0;
  filters_len_ = 0;
  filters_ = "";

  info_.Clear();

  linked_record_ = 0;
  link_name_len_ = 0;
  link_name_ = "";
  reference_box_id_ = 0;
}

}  // namespace genie::core::record::variant_site

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
