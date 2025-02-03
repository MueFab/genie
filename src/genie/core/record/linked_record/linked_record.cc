/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#include "linked_record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

LinkRecord::LinkRecord(genie::util::BitReader& bitreader){
    auto link_name_len = bitreader.ReadAlignedInt<uint8_t>();
    bitreader.ReadAlignedBytes(&link_name[0], link_name_len);
    reference_box_ID = bitreader.ReadAlignedInt<uint8_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LinkRecord::getLinkNameLen() const { return static_cast<uint8_t>(link_name.size()); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& LinkRecord::getLinkName() const { return link_name; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LinkRecord::getReferenceBoxID() const { return reference_box_ID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
