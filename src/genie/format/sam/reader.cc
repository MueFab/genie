/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/reader.h"
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Reader::Reader(std::istream& _stream, bool _with_index) : stream(_stream), header(stream), with_index(_with_index) {
    uint16_t seqID = 0;
    for (auto& headerLine : header.getLines()) {
        if (headerLine.getName() == "SQ") {
            for (auto& tag : headerLine.getTags()) {
                auto tagName = tag->getName();

                if (tagName == "SN") {
                    refs.emplace(tag->toString(), seqID);
                } else if (tagName == "AN") {
                    // Exclude Tag AN
                    // refs.emplace(tag->toString(), seqID);
                }
            }
            seqID++;
        }
    }

    if (with_index) {
        auto init_pos = stream.tellg();

        // Add entry to index
        while (stream.good()) {
            std::string str;
            size_t pos = stream.tellg();

            std::getline(stream, str, '\t');

            // If not a header, add to index
            UTILS_DIE_IF(str[0] == '@', "Found header in the middle of SAM file");

            // Handle case where there is empty line before EOF
            if (str.length()) {
                addCacheEntry(str, pos);
            }

            // Move to next line, skip the rest of fields
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        // Reset error flags then go back to initial position
        stream.clear();
        stream.seekg(init_pos);

        UTILS_DIE_IF(stream.tellg() != init_pos, "Unable to move file pointer");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const header::Header& Reader::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------
std::map<std::string, uint16_t>& Reader::getRefs() { return refs; }
// ---------------------------------------------------------------------------------------------------------------------

void Reader::addCacheEntry(std::string& qname, size_t& pos) {
    auto search = index.find(qname);
    if (!(search == index.end())) {
        search->second.push_back(pos);
    } else {
        index.emplace(std::move(qname), std::vector<size_t>({pos}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::read(std::list<std::string>& lines) {
    std::string line;

    if (with_index) {
        auto entry = index.begin();

        // while (!entry->second.empty()){
        for (auto& pos : entry->second) {
            stream.clear();

            stream.seekg(pos);
            std::getline(stream, line);
            lines.push_back(std::move(line));

            stream.clear();
        }

        index.erase(entry);
        return true;
    } else {
        UTILS_DIE_IF(!stream.good(), "Cannot read stream");
        std::getline(stream, line);

        lines.push_back(std::move(line));
        return true;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::good() { return (stream.good() && !(with_index && index.empty())); }

// ---------------------------------------------------------------------------------------------------------------------

ReadTemplateGroup::ReadTemplateGroup(bool _enable_window) : counter(0), enable_window(_enable_window) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addEntry(const std::string& qname) {
    if (enable_window) {
        window.emplace_front(qname, counter);
        counter++;
    }
}
// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::updateEntry(const std::string& qname) {
    if (enable_window) {
        bool found = false;
        for (auto iter = window.begin(); iter != window.end(); iter++) {
            if (iter->first == qname) {
                found = true;
                iter->second = counter;
                counter++;

                window.push_front(std::move(*iter));
                window.erase(iter);

                // window.splice(window.begin(), window, iter, std::next(iter));
                break;
            }
        }

        UTILS_DIE_IF(!found, "No read template with QNAME " + qname);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::removeEntry(const std::string& qname) {
    if (enable_window) {
        bool found = false;
        for (auto iter = window.begin(); iter != window.end(); iter++) {
            if (iter->first == qname) {
                found = true;
                window.erase(iter);
                break;
            }
        }

        UTILS_DIE_IF(!found, "No read template with QNAME " + qname);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addRecord(Record&& rec) {
    auto qname = rec.getQname();
    if (data.find(qname) == data.end()) {
        data.emplace(qname, ReadTemplate(std::move(rec)));
        addEntry(qname);
    } else {
        // Do not use data[] as it allows initialization
        data.at(qname).addRecord(std::move(rec));
        updateEntry((qname));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addRecords(std::list<Record>& recs) {
    for (auto& rec : recs) {
        addRecord(std::move(rec));
    }
    recs.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addRecords(std::vector<Record>& recs) {
    for (auto& rec : recs) {
        addRecord(std::move(rec));
    }
    recs.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addRecords(std::list<std::string>& lines) {
    for (auto& line : lines) {
        Record sam_rec(line);

        addRecord(std::move(sam_rec));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::resetCounter() {
    auto lowest_counter = window.back().second;
    counter -= lowest_counter;
    for (auto& entry : window) {
        entry.second -= lowest_counter;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReadTemplateGroup::getTemplate(ReadTemplate& t, const size_t& threshold) {
    if (enable_window) {
        if (counter - window.back().second > threshold) {
            if (!window.empty() && data[window.back().first].isValid()) {
                t = std::move(data[window.back().first]);
                data.erase(window.back().first);
                window.erase(std::prev(window.end()));

                removeEntry(t.getQname());
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        if (!data.empty()) {
            t = std::move(data.at(data.begin()->first));
            data.erase(t.getQname());
            return true;
        } else {
            return false;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::getTemplates(std::list<ReadTemplate>& ts, const size_t& threshold) {
    ts.clear();

    ReadTemplate t;
    while (getTemplate(t, threshold)) {
        ts.push_back(std::move(t));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
