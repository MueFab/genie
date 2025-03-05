/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/annotation_parameter_set/record.h"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/core/record/variant_site/record.h"
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

void Record::Write(core::Writer& writer) {
  writer.Write(variant_index_, 64);
  writer.Write(seq_id_, 16);
  writer.Write(pos_, 40);
  writer.Write(strand_, 8);
  writer.Write(id_len_, 8);
  writer.Write(id_);
  writer.Write(description_len_, 8);
  writer.Write(description_);
  writer.Write(ref_len_, 32);
  writer.Write(ref_);

  writer.Write(alt_count_, 8);
  for (auto i = 0; i < alt_count_; ++i) {
    writer.Write(alt_len_[i], 32);
    writer.Write(altern_[i]);
  }
  writer.Write(depth_, 32);
  writer.Write(seq_qual_, 32);
  writer.Write(map_qual_, 32);
  writer.Write(map_num_qual_0_, 32);
  writer.Write(filters_len_, 8);
  writer.Write(filters_);

  auto info_tag = info_.GetFields();
  writer.Write(static_cast<uint8_t>(info_tag.size()), 8);
  for (auto i = 0u; i < info_tag.size(); ++i) {
    writer.Write(info_tag[i].tag.size(), 8);
    writer.Write(info_tag[i].tag);
    writer.Write(static_cast<uint8_t>(info_tag[i].type), 8);
    writer.Write(info_tag[i].values.size(), 8);
    ArrayType writeType;
    for (auto j = 0u; j < info_tag[i].values.size(); ++j) {
      writeType.toFile(info_tag[i].type, info_tag.at(i).values.at(j), writer);
      if (info_tag[i].type == DataType::STRING)
        writer.WriteReserved(8);
    }
  }
  writer.WriteReserved(7);
  writer.Write(linked_record_, 1);
  if (linked_record_) {
    writer.Write(link_name_len_, 8);
    writer.Write(link_name_);
    writer.Write(reference_box_id_, 8);
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
  for (auto i = 0; i < alt_count_; ++i) {
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
