#include "tag-string-list.h"
#include <util/tokenize.h>

namespace format {
namespace sam {

TagStringList::TagStringList(std::string _name, std::vector<std::string> _data, char _delimiter)
    : TagBase(std::move(_name)), data(std::move(_data)), delimiter(_delimiter) {}

std::string TagStringList::toString() const {
    std::string ret;
    for (const auto& s : data) {
        ret += s;
        ret += delimiter;
    }
    ret.pop_back();
    return ret;
}

std::unique_ptr<TagBase> TagStringList::fromString(std::string name, const std::string& string, char delimiter) {
    return util::make_unique<TagStringList>(std::move(name), tokenize(string, delimiter), delimiter);
}
}  // namespace sam
}  // namespace format