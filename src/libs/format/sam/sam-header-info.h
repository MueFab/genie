#ifndef GENIE_SAM_HEADER_INFO_H
#define GENIE_SAM_HEADER_INFO_H

#include <functional>
#include <memory>
#include <string>
#include "sam-tag.h"

namespace format {
namespace sam {
struct TagInfo {
   public:
    std::string name;
    bool required;
    std::string regex;
    std::function<std::unique_ptr<TagBase>(std::string, const std::string&)> create;
};

struct HeaderLineInfo {
    std::string name;
    std::vector<TagInfo> tags;
};

const std::vector<HeaderLineInfo>& getHeaderInfo();
}  // namespace sam
}  // namespace format

#endif  // GENIE_SAM_HEADER_INFO_H
