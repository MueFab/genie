/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_HEADER_H
#define GENIE_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "header-info.h"
#include "tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

/**
 *
 */
class HeaderLine {
   private:
    std::string name;                            //!<
    std::string comment;                         //!<
    std::vector<std::unique_ptr<TagBase>> tags;  //!<

    /**
     *
     * @param value
     * @param info
     */
    void parseSingleTag(const std::string& value, const TagInfo& info);

    /**
     *
     * @param _tags
     * @param info
     */
    void parseTags(const std::vector<std::string>& _tags, const HeaderLineInfo& info);

   public:
    /**
     *
     * @return
     */
    const std::string& getName() const;

    /**
     *
     * @return
     */
    const std::string& getComment() const;

    /**
     *
     * @return
     */
    const std::vector<std::unique_ptr<TagBase>>& getTags() const;

    /**
     *
     * @param tag
     */
    void addTag(std::unique_ptr<TagBase> tag);

    /**
     *
     * @param line
     */
    explicit HeaderLine(const std::string& line);

    /**
     *
     * @param line
     */
    HeaderLine(HeaderLine&& line) noexcept;

    /**
     *
     * @param _name
     * @param _comment
     */
    HeaderLine(std::string&& _name, std::string&& _comment);

    /**
     *
     * @param stream
     */
    void print(std::ostream& stream) const;
};

/**
 *
 */
class Header {
   private:
    std::vector<HeaderLine> lines;  //!<

    /**
     *
     */
    void globalChecks() const;

   public:
    /**
     *
     * @param stream
     */
    explicit Header(std::istream& stream);

    /**
     *
     */
    Header() = default;

    /**
     *
     * @param header
     */
    Header(Header&& header) noexcept;

    /**
     *
     * @param stream
     */
    void print(std::ostream& stream) const;

    /**
     *
     * @return
     */
    const std::vector<HeaderLine>& getLines() const;

    /**
     *
     * @param line
     */
    void addLine(HeaderLine&& line);

    /**
     *
     * @return
     */
    static Header createDefaultHeader();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------