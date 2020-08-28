/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <limits>
#include <numeric>
#include <algorithm>

#include "reader.h"
#include <genie/util/runtime-exception.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Reader::Reader(std::istream& _stream, bool _with_index) : stream(_stream), header(stream), with_index(_with_index) {

    uint16_t seqID = 0;
    for (auto &headerLine : header.getLines()){
        if (headerLine.getName() == "SQ") {
            for (auto &tag : headerLine.getTags()) {
                auto tagName = tag->getName();

                if (tagName == "SN"){
                    refs.emplace(tag->toString(), seqID);
                } else if(tagName == "AN"){
                    // Exclude Tag AN
                    //refs.emplace(tag->toString(), seqID);
                }
            }
            seqID++;
        }
    }

    if (with_index){
        int init_pos = stream.tellg();
        std::string str;
        std::string flag;
        std::string mapping_pos;

        // Add entry to index
        while(stream.good()){

            size_t pos = stream.tellg();

            std::getline(stream, str, '\t');

            // check FLAG
            // stream.ignore(std::numeric_limits<std::streamsize>::max(), '\t');
            std::getline(stream, flag, '\t');

            // Ignore RNAME
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\t');

            std::getline(stream, mapping_pos, '\t');

            // If not a header, add to index
            UTILS_DIE_IF(str[0] == '@', "Found header in the middle of SAM file");

            uint16_t flag_uint = std::stoi(flag);
            size_t mpos = std::stoi(mapping_pos);

            // Handle case where there is empty line before EOF
            if (str.length()){
                addCacheEntry(pos, str, mpos,
//                      !(flag_uint & 900)
                    !(flag_uint & (1u << uint8_t(Record::FlagPos::SECONDARY_ALIGNMENT))) &&
                                     !(flag_uint & (1u << uint8_t(Record::FlagPos::SUPPLEMENTARY_ALIGNMENT)))
                );
            }

            // Move to next line, skip the rest of fields
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        }

        // Reset error flags then go back to initial position
        stream.clear();
        stream.seekg(init_pos);

        UTILS_DIE_IF(stream.tellg() != init_pos, "Unable to move file pointer");

        std::vector<std::size_t> tmp_pos_list;
        /// Sorting QNAME based on its mapping position
        for (auto & it : index) {
            tmp_pos_list.push_back(it.second.second);
            qname_list.push_back(it.first);
        }

        // Insert value from 0 to tmp_pos_list.size()
        ids.resize(tmp_pos_list.size());
        std::iota(ids.begin(), ids.end(), 0);

        if (ids.size() > 1){
            std::sort(ids.begin(), ids.end(), [&tmp_pos_list](size_t i1, size_t i2) {
                if (i1 == 0){
                    return false;
                } else if (i2 == 0){
                    return true;
                };
                return tmp_pos_list[i1] < tmp_pos_list[i2];
            });
        }

//        std::move(tmp_pos_list.begin(), tmp_pos_list.end(), std::back_inserter(ids));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const header::Header& Reader::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

std::map<std::string, size_t>& Reader::getRefs() { return refs; }

// ---------------------------------------------------------------------------------------------------------------------

void Reader::addCacheEntry(size_t pos, std::string &qname,  size_t mapping_pos, bool update_mapping_pos) {
    auto search = index.find(qname);
    if (!(search == index.end())) {
        search->second.first.push_back(pos);

        if (update_mapping_pos && search->second.second > mapping_pos){
            search->second.second = mapping_pos;
        }

    } else {
        std::pair<std::vector<size_t>, size_t> data({pos}, mapping_pos);
        index.emplace(std::move(qname), std::move(data));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::read(std::list<std::string>& lines) {
    std::string line;

    if (with_index){

        if (ids.empty()) {
            return false;
        }

        auto entry = index.find(qname_list[ids.front()]);

        if (!(entry == index.end())) {

            for (auto& pos : entry->second.first) {
                stream.clear();

                stream.seekg(pos);
                std::getline(stream, line);
                lines.push_back(std::move(line));

                stream.clear();
            }

            index.erase(entry);
            ids.erase(ids.begin());

            return true;

        } else {
            return false;
        }
    } else {
        UTILS_DIE_IF(!stream.good(), "Cannot read stream");
        std::getline(stream, line);

        lines.push_back(std::move(line));
        return true;
    }

}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::good() {
    return (stream.good() && !(with_index && index.empty()));
}

// ---------------------------------------------------------------------------------------------------------------------

ReadTemplateGroup::ReadTemplateGroup(bool _enable_window) : counter(0), enable_window(_enable_window) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::addEntry(const std::string& qname) {
    if (enable_window){
        window.emplace_front(qname, counter);
        counter++;
    }
}
// ---------------------------------------------------------------------------------------------------------------------

void ReadTemplateGroup::updateEntry(const std::string& qname) {
    if (enable_window){
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

void ReadTemplateGroup::addRecords(std::list<std::string>& lines) {
    for (auto &line: lines){
        Record sam_rec(line);

        addRecord(std::move(sam_rec));
    }
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

bool ReadTemplateGroup::getTemplate(ReadTemplate& t, const size_t& threshold) {
    if (enable_window) {
        if (counter - window.back().second > threshold) {
            if (!window.empty() && data[window.back().first].isValid()){
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
        if (!data.empty()){
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