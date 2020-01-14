#include "tag-string.h"

namespace genie {
namespace sam {
TagString::TagString(std::string _name, std::string _data) : TagBase(std::move(_name)), data(std::move(_data)) {}

std::string TagString::toString() const { return data; }

std::unique_ptr<TagBase> TagString::fromString(std::string name, const std::string& string) {
    return util::make_unique<TagString>(std::move(name), string);
}
}  // namespace sam
}  // namespace genie