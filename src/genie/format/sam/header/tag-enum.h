/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TAG_ENUM_H
#define GENIE_TAG_ENUM_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/runtime-exception.h>
#include <genie/util/make-unique.h>
#include "tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

/**
 *
 * @tparam Type
 */
template <typename Type>
class TagEnum : public TagBase {
   private:
    Type data;                               //!<
    const std::vector<std::string>& lookup;  //!<

   public:
    /**
     *
     * @param _name
     * @param _data
     * @param _lookup
     */
    TagEnum(std::string _name, Type _data, const std::vector<std::string>& _lookup)
        : TagBase(std::move(_name)), data(_data), lookup(_lookup) {}

    /**
     *
     * @return
     */
    std::string toString() const override { return lookup[size_t(data)]; }

    /**
     *
     * @return
     */
    Type getData() const { return data; }

    /**
     *
     * @param name
     * @param string
     * @param lookup
     * @return
     */
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

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TAG_ENUM_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------