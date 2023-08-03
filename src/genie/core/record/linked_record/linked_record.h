/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_CORE_RECORD_LINKED_RECORD_LINKED_RECORD_H_
#define GENIE_CORE_RECORD_LINKED_RECORD_LINKED_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

class LinkRecord {
 private:
    std::string link_name;     //!< @brief
    uint8_t reference_box_ID;  //!< @brief
 public:
    explicit LinkRecord(util::BitReader& bitreader);

    uint8_t getLinkNameLen() const;
    const std::string& getLinkName() const;
    uint8_t getReferenceBoxID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CORE_RECORD_LINKED_RECORD_LINKED_RECORD_H_
