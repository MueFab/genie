#include "tag-number.h"

namespace format {
namespace sam {
TagNumber::TagNumber(std::string _name, uint64_t _data) : TagBase(std::move(_name)), data(_data) {}

std::string TagNumber::toString() const { return std::to_string(data); }

std::unique_ptr<TagBase> TagNumber::fromString(std::string name, const std::string& string) {
    return util::make_unique<TagNumber>(std::move(name), std::stoi(string));
}
}  // namespace sam
}  // namespace format