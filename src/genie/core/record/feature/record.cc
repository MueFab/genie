/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/feature/record.h"

// -------------------------------------------------------------------------------------------------

namespace genie::core::record::feature {

// -------------------------------------------------------------------------------------------------

Record::Record(util::BitReader& reader) {
  Read(reader);
}

// -------------------------------------------------------------------------------------------------

void Record::Write(util::BitWriter& writer) {
    (void)writer;
}

// -------------------------------------------------------------------------------------------------

bool Record::Read(util::BitReader& reader) {
  ClearData();
  feature_index_ = reader.ReadBits(64);
  if (!reader.IsStreamGood())
    return false;
  feature_attributes_.Read(reader);
  reader.ReadBits(7);
  linked_record_ = reader.ReadBits(1);
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
  feature_index_ = 0;
  feature_attributes_.Clear();
  linked_record_ = 0;
  link_name_len_ = 0;
  link_name_ = "";
  reference_box_id_ = 0;
}

}  // namespace genie::core::record::feature
