/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_HEADER_TAG_STRING_LIST_H_
#define SRC_GENIE_FORMAT_SAM_HEADER_TAG_STRING_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/sam/header/tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

/**
 *
 */
class TagStringList : public TagBase {
 private:
    std::vector<std::string> data;  //!<
    char delimiter;                 //!<

 public:
    /**
     *
     * @param _name
     * @param _data
     * @param _delimiter
     */
    TagStringList(std::string _name, std::vector<std::string> _data, char _delimiter);

    /**
     *
     * @return
     */
    std::string toString() const override;

    /**
     *
     * @param name
     * @param string
     * @param delimiter
     * @return
     */
    static std::unique_ptr<TagBase> fromString(std::string name, const std::string& string, char delimiter);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_HEADER_TAG_STRING_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
