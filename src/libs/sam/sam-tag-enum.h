#ifndef GENIE_SAM_TAG_ENUM_H
#define GENIE_SAM_TAG_ENUM_H

#include "sam-tag.h"

namespace genie {
namespace sam {
template <typename Type>
class TagEnum : public TagBase {
   private:
    Type data;
    const std::vector<std::string>& lookup;

   public:
    TagEnum(std::string _name, Type _data, const std::vector<std::string>& _lookup)
        : TagBase(std::move(_name)), data(_data), lookup(_lookup) {}

    std::string toString() const override { return lookup[size_t(data)]; }

    Type getData() const { return data; }

    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string,
                                               const std::vector<std::string>& lookup) {
        for (size_t s = 0; s < lookup.size(); ++s) {
            if (string == lookup[s]) {
                return util::make_unique<TagEnum<Type>>(std::move(name), Type(s), lookup);
            }
        }
        UTILS_DIE("Unknown Tag value");
    }
};
}  // namespace sam
}  // namespace genie

#endif  // GENIE_SAM_TAG_ENUM_H
