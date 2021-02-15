/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_HEADER_HEADER_INFO_H_
#define SRC_GENIE_FORMAT_SAM_HEADER_HEADER_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "genie/format/sam/header/tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

struct TagInfo {
 public:
    std::string name;
    bool required;
    std::string regex;
    std::function<std::unique_ptr<TagBase>(std::string, const std::string&)> create;
};

// ---------------------------------------------------------------------------------------------------------------------

struct HeaderLineInfo {
    std::string name;
    std::vector<TagInfo> tags;
};

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<HeaderLineInfo>& getHeaderInfo();

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_HEADER_HEADER_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
