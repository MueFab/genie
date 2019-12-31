#ifndef GENIE_TAG_STRING_LIST_H
#define GENIE_TAG_STRING_LIST_H

#include "sam-tag.h"

namespace format {
namespace sam {
class TagStringList : public TagBase {
   private:
    std::vector<std::string> data;
    char delimiter;

   public:
    TagStringList(std::string _name, std::vector<std::string> _data, char _delimiter);

    std::string toString() const override;

    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string, char delimiter);
};
}  // namespace sam
}  // namespace format

#endif  // GENIE_TAG_STRING_LIST_H
