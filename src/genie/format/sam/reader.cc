/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reader.h"
#include <genie/util/exceptions.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Reader::Reader(std::istream& _stream) : stream(_stream), header(stream), rec_saved(false) {}

// ---------------------------------------------------------------------------------------------------------------------

const header::Header& Reader::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::read(Record& rec) {
    std::string str;
    if (std::getline(stream, str)){
        rec = Record(str);
        return true;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Reader::readRecords(size_t num, std::vector<Record>& vec) {
    Record rec;
    while (num && read(rec)){
        vec.push_back(std::move(rec));
        num--;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::isEnd() { return stream.eof(); }

// ---------------------------------------------------------------------------------------------------------------------

ReadTemplateGroup::ReadTemplateGroup() : counter(0) {}

void ReadTemplateGroup::addEntry(const std::string& qname) {
    window.emplace_front(qname, counter);
    counter++;
}

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

    //TODO (yeremia): Fix error msg
    UTILS_DIE_IF(!found, "???");
}

void ReadTemplateGroup::removeEntry(std::string& qname) {
    bool found = false;
    for (auto iter = window.begin(); iter != window.end(); iter++){
        if (iter->first == qname){
            found = true;
            window.erase(iter);
            break;
        }
    }
    //TODO (yeremia): Fix error msg
    UTILS_DIE_IF(!found, "???");
}

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
void ReadTemplateGroup::addRecords(std::list<Record>& recs) {
    for (auto & rec : recs){
        addRecord(std::move(rec));
    }
    recs.clear();
}

void ReadTemplateGroup::addRecords(std::vector<Record>& recs) {
    for (auto & rec : recs){
        addRecord(std::move(rec));
    }
    recs.clear();
}

void ReadTemplateGroup::resetCounter() {
    auto lowest_counter = window.back().second;
    counter -= lowest_counter;
    for (auto & entry : window){
        entry.second -= lowest_counter;
    }
}
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
bool ReadTemplateGroup::getTemplate(ReadTemplate& t, const size_t& threshold) {
    if (counter - window.back().second > threshold) {
        return getTemplate(t);
    } else {
        return false;
    }
}
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