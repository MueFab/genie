/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reader.h"
#include <genie/util/exceptions.h>

#include <limits>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Reader::Reader(std::istream& _stream, bool _with_cache) : stream(_stream), header(stream), with_cache(_with_cache) {
    if (with_cache){
        int init_pos = stream.tellg();

        // Add entry to cache
        while(stream.good()){
            std::string str;
            size_t pos = stream.tellg();

            std::getline(stream, str, '\t');

            // If not a header, add to tache
            if (str[0] != '@'){
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

void Reader::addCacheEntry(std::string &qname, size_t &pos) {
    auto search = cache.find(qname);
    if (!(search == cache.end())) {
        search->second.push_back(pos);
    } else {
        cache.emplace(std::move(qname), std::vector<size_t>({pos}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::read(std::list<std::string> lines) {
    std::string line;

    if (!good()){
        return false;
    }

    if (with_cache){
        auto entry = cache.begin();

        while (!entry->second.empty()){
            stream.seekg(entry->second.front());
            std::getline(stream, line);
            lines.push_back(std::move(line));

            stream.clear();
        }

        cache.erase(entry);
        return true;
    } else {
        std::getline(stream, line);

        lines.push_back(std::move(line));
        return true;
    }

}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::good() {
    return (stream.good() && !(with_cache && cache.empty()));
}

// ---------------------------------------------------------------------------------------------------------------------

ReadTemplateGroup::ReadTemplateGroup() : counter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addEntry(const std::string& qname) {
    window.emplace_front(qname, counter);
    counter++;
}
// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::updateEntry(const std::string& qname) {
    bool found = false;
    for (auto iter = window.begin(); iter != window.end(); iter++){
        if (iter->first == qname){
            found = true;
            iter->second = counter;
            counter ++;

            window.push_front(std::move(*iter));
            window.erase(iter);

            //window.splice(window.begin(), window, iter, std::next(iter));
            break;
        }
    }

    UTILS_DIE_IF(!found, "No read template with QNAME " + qname);
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::removeEntry(std::string& qname) {
    bool found = false;
    for (auto iter = window.begin(); iter != window.end(); iter++){
        if (iter->first == qname){
            found = true;
            window.erase(iter);
            break;
        }
    }

    UTILS_DIE_IF(!found, "No read template with QNAME " + qname);
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addRecord(Record&& rec) {
    auto qname = rec.getQname();
    if (data.find(qname) == data.end()){
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
    for (auto & rec : recs){
        addRecord(std::move(rec));
    }
    recs.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addRecords(std::vector<Record>& recs) {
    for (auto & rec : recs){
        addRecord(std::move(rec));
    }
    recs.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::resetCounter() {
    auto lowest_counter = window.back().second;
    counter -= lowest_counter;
    for (auto & entry : window){
        entry.second -= lowest_counter;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReadTemplateGroup::getTemplate(ReadTemplate& t) {
    if (!window.empty() && data[window.back().first].isValid()){
        t = std::move(data[window.back().first]);
        data.erase(window.back().first);
        window.erase(std::prev(window.end()));
        return true;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReadTemplateGroup::getTemplate(ReadTemplate& t, const size_t& threshold) {
    if (counter - window.back().second > threshold) {
        return getTemplate(t);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::getTemplates(std::list<ReadTemplate>& ts, const size_t& threshold) {
    ts.clear();

    ReadTemplate t;
    while (getTemplate(t, threshold)){
        ts.push_back(std::move(t));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------