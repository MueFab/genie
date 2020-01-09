#include "sam-header.h"
#include <util/tokenize.h>

namespace format {
namespace sam {
const std::string& SamHeaderLine::getName() const { return name; }

const std::string& SamHeaderLine::getComment() const { return comment; }

const std::vector<std::unique_ptr<TagBase>>& SamHeaderLine::getTags() const { return tags; }

void SamHeaderLine::addTag(std::unique_ptr<TagBase> tag) { tags.push_back(std::move(tag)); }

void SamHeaderLine::parseSingleTag(const std::string& value, const TagInfo& info) {
    if (!info.regex.empty()) {
        const std::regex regex(info.regex);
        if (!std::regex_match(value, regex)) {
            std::cerr << "Sam header regex fail: skipping tag " << info.name << std::endl;
            return;
        }
    }
    auto tag = info.create(info.name, value);
    tags.emplace_back(std::move(tag));
}
void SamHeaderLine::parseTags(const std::vector<std::string>& _tags, const HeaderLineInfo& info) {
    bool first = true;
    for (const auto& t : _tags) {
        if (first) {
            first = false;
            continue;
        }
        auto separator = t.find(':');
        if (separator == std::string::npos) {
            UTILS_DIE("Tag separator not found in sam header");
        }
        std::string tagname = t.substr(0, separator);
        std::string tagvalue = t.substr(separator + 1);
        bool found = false;
        for (const auto& taginfo : info.tags) {
            if (taginfo.name == tagname) {
                found = true;
                parseSingleTag(tagvalue, taginfo);
            }
        }
        if (!found) {
            UTILS_DIE("Tag unknown in sam header");
        }
    }
}
SamHeaderLine::SamHeaderLine(const std::string& line) {
    auto ret = tokenize(line, '\t');
    name = ret.front().substr(1);
    const std::string COMMENT = "CO";
    if (name == COMMENT) {
        comment = line.substr(4);
        return;
    }
    bool found = false;
    for (const auto& header : getHeaderInfo()) {
        if (header.name == name) {
            found = true;
            parseTags(ret, header);
        }
    }
    if (!found) {
        UTILS_DIE("Unknown Sam header line");
    }
}

SamHeaderLine::SamHeaderLine(SamHeaderLine&& line) noexcept
    : name(std::move(line.name)), comment(std::move(line.comment)), tags(std::move(line.tags)) {}

SamHeaderLine::SamHeaderLine(std::string&& _name, std::string&& _comment)
    : name(std::move(_name)), comment(std::move(_comment)) {}

void SamHeaderLine::print(std::ostream& stream) const {
    stream << "@" << name;
    for (const auto& t : tags) {
        stream << "\t" << t->getName() << ":" << t->toString();
    }
    stream << "\n";
}

SamFileHeader::SamFileHeader(std::istream& stream) {
    while (true) {
        if (stream.peek() != '@') {
            break;
        }
        std::string samline;
        std::getline(stream, samline);
        lines.emplace_back(samline);
    }
}

SamFileHeader::SamFileHeader(SamFileHeader&& header) noexcept : lines(std::move(header.lines)) {}

void SamFileHeader::print(std::ostream& stream) const {
    for (const auto& l : lines) {
        l.print(stream);
    }
}

const std::vector<SamHeaderLine>& SamFileHeader::getLines() const { return lines; }
void SamFileHeader::addLine(SamHeaderLine&& line) { lines.emplace_back(std::move(line)); }
void SamFileHeader::globalChecks() const {
    // TODO
}

}  // namespace sam
}  // namespace format