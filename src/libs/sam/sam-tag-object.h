#ifndef GENIE_SAM_TAG_OBJECT_H
#define GENIE_SAM_TAG_OBJECT_H

#include "sam-tag.h"

namespace genie {
namespace sam {
template <typename Type>
class TagObject : public TagBase {
   private:
    Type data;

   public:
    const Type& getData() const { return data; }

    TagObject(std::string _name, Type _data) : TagBase(std::move(_name)), data(_data) {}

    std::string toString() const override { return data.toString(); }

    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string) {
        return util::make_unique<TagObject<Type>>(std::move(name), Type::fromString(string));
    }
};
}  // namespace sam
}  // namespace genie

#endif  // GENIE_SAM_TAG_OBJECT_H
