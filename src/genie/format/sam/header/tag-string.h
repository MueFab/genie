/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_HEADER_TAG_STRING_H_
#define SRC_GENIE_FORMAT_SAM_HEADER_TAG_STRING_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "genie/format/sam/header/tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

class TagString : public TagBase {
 private:
    std::string data;

 public:
    TagString(std::string _name, std::string _data);

    std::string toString() const override;

    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_HEADER_TAG_STRING_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
