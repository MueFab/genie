/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/linked/record.h"

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::linked {

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(genie::util::BitReader& bitreader) {
    auto link_name_len = bitreader.ReadAlignedInt<uint8_t>();
    bitreader.ReadAlignedBytes(&link_name_[0], link_name_len);
    reference_box_id_ = bitreader.ReadAlignedInt<uint8_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Record::GetLinkNameLen() const {
    return static_cast<uint8_t>(link_name_.size());
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Record::GetLinkName() const {
    return link_name_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Record::GetReferenceBoxId() const {
    return reference_box_id_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool operator==(const Record& lhs, const Record& rhs) {
    return lhs.GetLinkName() == rhs.GetLinkName() &&
           lhs.GetReferenceBoxId() == rhs.GetReferenceBoxId();
}

}  // namespace genie::core::record::linked

// ---------------------------------------------------------------------------------------------------------------------
