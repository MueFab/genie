/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "header.h"
#include <genie/util/tokenize.h>
#include <iostream>
#include <regex>
#include "tag-enum.h"
#include "tag-number.h"
#include "tag-string.h"
#include "tag-version.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& HeaderLine::getName() const { return name; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& HeaderLine::getComment() const { return comment; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<TagBase>>& HeaderLine::getTags() const { return tags; }

// ---------------------------------------------------------------------------------------------------------------------

void HeaderLine::addTag(std::unique_ptr<TagBase> tag) { tags.push_back(std::move(tag)); }

// ---------------------------------------------------------------------------------------------------------------------

void HeaderLine::parseSingleTag(const std::string& value, const TagInfo& info) {
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

// ---------------------------------------------------------------------------------------------------------------------

void HeaderLine::parseTags(const std::vector<std::string>& _tags, const HeaderLineInfo& info) {
    bool first = true;
    for (const auto& t : _tags) {
        if (first) {
            first = false;
            continue;
        }
        auto separator = t.find(':');
        UTILS_DIE_IF(separator == std::string::npos, "Tag separator not found in sam header");
        std::string tagname = t.substr(0, separator);
        std::string tagvalue = t.substr(separator + 1);
        bool found = false;
        for (const auto& taginfo : info.tags) {
            if (taginfo.name == tagname) {
                found = true;
                parseSingleTag(tagvalue, taginfo);
            }
        }
        UTILS_DIE_IF(!found, "Tag unknown in sam header");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

HeaderLine::HeaderLine(const std::string& line) {
    auto ret = util::tokenize(line, '\t');
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
    UTILS_DIE_IF(!found, "Unknown Sam header line");
}

// ---------------------------------------------------------------------------------------------------------------------

HeaderLine::HeaderLine(HeaderLine&& line) noexcept
    : name(std::move(line.name)), comment(std::move(line.comment)), tags(std::move(line.tags)) {}

// ---------------------------------------------------------------------------------------------------------------------

HeaderLine::HeaderLine(std::string&& _name, std::string&& _comment)
    : name(std::move(_name)), comment(std::move(_comment)) {}

// ---------------------------------------------------------------------------------------------------------------------

void HeaderLine::print(std::ostream& stream) const {
    stream << "@" << name;
    for (const auto& t : tags) {
        stream << "\t" << t->getName() << ":" << t->toString();
    }
    stream << "\n";
}

// ---------------------------------------------------------------------------------------------------------------------

Header::Header(std::istream& stream) {
    while (true) {
        if (stream.peek() != '@') {
            break;
        }
        std::string samline;
        std::getline(stream, samline);
        lines.emplace_back(samline);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Header::Header(Header&& header) noexcept : lines(std::move(header.lines)) {}

// ---------------------------------------------------------------------------------------------------------------------

void Header::print(std::ostream& stream) const {
    for (const auto& l : lines) {
        l.print(stream);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<HeaderLine>& Header::getLines() const { return lines; }

// ---------------------------------------------------------------------------------------------------------------------

void Header::addLine(HeaderLine&& line) { lines.emplace_back(std::move(line)); }

// ---------------------------------------------------------------------------------------------------------------------

void Header::globalChecks() const {
    // TODO
}

// ---------------------------------------------------------------------------------------------------------------------

Header Header::createDefaultHeader() {
    Header header;
    HeaderLine hd("HD", "");
    hd.addTag(util::make_unique<TagVersion>("VN", std::array<uint8_t, 2>{1, 6}));
    hd.addTag(util::make_unique<TagEnum<SortingOrder>>("SO", SortingOrder::COORDINATE, getSortingOrderString()));
    header.addLine(std::move(hd));
    HeaderLine sq("SQ", "");
    sq.addTag(util::make_unique<TagString>("SN", "GenieTestRef"));
    sq.addTag(util::make_unique<TagNumber>("LN", 123456));
    sq.addTag(util::make_unique<TagString>("SP", "Anas-platyrhynchos"));
    header.addLine(std::move(sq));
    HeaderLine pg("PG", "");
    pg.addTag(util::make_unique<TagString>("ID", "Genie"));
    header.addLine(std::move(pg));
    HeaderLine rg("RG", "");
    rg.addTag(util::make_unique<TagString>("ID", "GenieGroup"));
    header.addLine(std::move(rg));
    return header;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------