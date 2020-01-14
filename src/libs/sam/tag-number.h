#ifndef GENIE_TAG_NUMBER_H
#define GENIE_TAG_NUMBER_H

#include "sam-tag.h"

namespace genie {
namespace sam {
class TagNumber : public TagBase {
   private:
    uint64_t data;

   public:
    TagNumber(std::string _name, uint64_t _data);

    std::string toString() const override;

    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string);
};
}  // namespace sam
}  // namespace genie

#endif  // GENIE_TAG_NUMBER_H
