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

namespace format {
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

    static SamFileHeader createDefaultHeader() {
        SamFileHeader header;
        SamHeaderLine hd("HD", "");
        hd.addTag(util::make_unique<TagVersion>("VN", std::array<uint8_t, 2>{1, 6}));
        hd.addTag(util::make_unique<TagEnum<SortingOrder>>("SO", SortingOrder::COORDINATE, getSortingOrderString()));
        header.addLine(std::move(hd));
        SamHeaderLine sq("SQ", "");
        sq.addTag(util::make_unique<TagString>("SN", "GenieTestRef"));
        sq.addTag(util::make_unique<TagNumber>("LN", 123456));
        sq.addTag(util::make_unique<TagString>("SP", "Anas-platyrhynchos"));
        header.addLine(std::move(sq));
        SamHeaderLine pg("PG", "");
        pg.addTag(util::make_unique<TagString>("ID", "Genie"));
        header.addLine(std::move(pg));
        SamHeaderLine rg("RG", "");
        rg.addTag(util::make_unique<TagString>("ID", "GenieGroup"));
        header.addLine(std::move(rg));
        return header;
    }
};
}  // namespace sam
}  // namespace format

#endif  // GENIE_SAM_HEADER_H
