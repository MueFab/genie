#ifndef GENIE_SAM_HEADER_H
#define GENIE_SAM_HEADER_H

#include <memory>
#include <string>
#include <vector>
#include "sam-header-info.h"
#include "sam-tag-enum.h"
#include "sam-tag.h"
#include "tag-number.h"
#include "tag-string.h"
#include "tag-version.h"
namespace genie {
namespace sam {
class SamHeaderLine {
   private:
    std::string name;
    std::string comment;
    std::vector<std::unique_ptr<TagBase>> tags;

    void parseSingleTag(const std::string& value, const TagInfo& info);
    void parseTags(const std::vector<std::string>& _tags, const HeaderLineInfo& info);

   public:
    const std::string& getName() const;
    const std::string& getComment() const;
    const std::vector<std::unique_ptr<TagBase>>& getTags() const;
    void addTag(std::unique_ptr<TagBase> tag);

    explicit SamHeaderLine(const std::string& line);
    SamHeaderLine(SamHeaderLine&& line) noexcept;
    SamHeaderLine(std::string&& _name, std::string&& _comment);

    void print(std::ostream& stream) const;
};

class SamFileHeader {
   private:
    std::vector<SamHeaderLine> lines;

    void globalChecks() const;

   public:
    explicit SamFileHeader(std::istream& stream);
    SamFileHeader() = default;
    SamFileHeader(SamFileHeader&& header) noexcept;
    void print(std::ostream& stream) const;

    const std::vector<SamHeaderLine>& getLines() const;
    void addLine(SamHeaderLine&& line);

    static SamFileHeader createDefaultHeader();
};
}  // namespace sam
}  // namespace genie
#endif  // GENIE_SAM_HEADER_H
