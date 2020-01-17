/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TAG_OBJECT_H
#define GENIE_TAG_OBJECT_H

// ---------------------------------------------------------------------------------------------------------------------

#include "tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TAG_OBJECT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------