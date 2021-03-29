/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_HEADER_TAG_VERSION_H_
#define SRC_GENIE_FORMAT_SAM_HEADER_TAG_VERSION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <memory>
#include <string>
#include "genie/format/sam/header/tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

/**
 *
 */
class TagVersion : public TagBase {
 private:
    std::array<uint8_t, 2> data;  //!<

 public:
    /**
     *
     * @param _name
     * @param _data
     */
    TagVersion(std::string _name, std::array<uint8_t, 2> _data);

    /**
     *
     * @return
     */
    std::string toString() const override;

    /**
     *
     * @param name
     * @param string
     * @return
     */
    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_HEADER_TAG_VERSION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
