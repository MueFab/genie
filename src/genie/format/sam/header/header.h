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
#include <map>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

class HeaderLine {
   private:
    std::string name;
    std::vector<std::unique_ptr<TagBase>> tags;

    void parseSingleTag(const std::string& value, const TagInfo& info);
    void parseTags(const std::vector<std::string>& _tags, const HeaderLineInfo& info);

   public:
    const std::string& getName() const;

    const std::vector<std::unique_ptr<TagBase>>& getTags() const;
    void addTag(std::unique_ptr<TagBase> tag);
    std::vector<std::unique_ptr<TagBase>>&& moveTags();

    explicit HeaderLine(const std::string& line);
    HeaderLine(HeaderLine&& line) noexcept;
    HeaderLine(std::string&& _name, std::string&& _comment);

    void print(std::ostream& stream) const;
};

// ---------------------------------------------------------------------------------------------------------------------

// Store parsed header lines
class Header {
   private:
    std::vector<HeaderLine> lines;
    std::vector<std::string> comments;
    std::map<std::string, std::vector<std::unique_ptr<TagBase>>> references;

    void globalChecks() const;

   public:
    explicit Header(std::istream& stream);
    Header() = default;
    Header(Header&& header) noexcept;
    void print(std::ostream& stream) const;

    const std::vector<HeaderLine>& getLines() const;
    void addReference(HeaderLine&& line);
    bool isReferenceExists(const std::string& _rname);
    void addComment(std::string&& _str);
    void addLine(HeaderLine&& line);

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