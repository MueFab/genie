/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "linked_record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

LinkRecord::LinkRecord(genie::util::BitReader& bitreader) {
  auto link_name_len = bitreader.ReadAlignedInt<uint8_t>();
  bitreader.ReadAlignedBytes(&link_name_[0], link_name_len);
  reference_box_id_ = bitreader.ReadAlignedInt<uint8_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LinkRecord::GetLinkNameLen() const {
  return static_cast<uint8_t>(link_name_.size());
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& LinkRecord::GetLinkName() const {
  return link_name_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LinkRecord::GetReferenceBoxId() const {
  return reference_box_id_;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------
