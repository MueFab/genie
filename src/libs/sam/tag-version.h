#ifndef GENIE_TAG_VERSION_H
#define GENIE_TAG_VERSION_H

#include <array>
#include "sam-tag.h"

namespace genie {
namespace sam {

class TagVersion : public TagBase {
   private:
    std::array<uint8_t, 2> data;

   public:
    TagVersion(std::string _name, std::array<uint8_t, 2> _data);

    std::string toString() const override;

    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string);
};
}  // namespace sam
}  // namespace genie

#endif  // GENIE_TAG_VERSION_H
