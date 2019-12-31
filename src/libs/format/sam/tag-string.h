#ifndef GENIE_TAG_STRING_H
#define GENIE_TAG_STRING_H

#include "sam-tag.h"

namespace format {
namespace sam {
class TagString : public TagBase {
   private:
    std::string data;

   public:
    TagString(std::string _name, std::string _data);

    std::string toString() const override;

    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string);
};
}  // namespace sam
}  // namespace format

#endif  // GENIE_TAG_STRING_H
