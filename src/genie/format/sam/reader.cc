/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reader.h"
#include <map>
#include <genie/util/exceptions.h>
#include <list>

#define WITH_CACHE true

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

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

bool Reader::isReferenceExists(const std::string& rname) {
    return ref_sequences.find(rname) != ref_sequences.end();
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::getSeqID(const std::string &rname, uint16_t &seqID) const {
    auto search =  ref_sequences.find(rname);

    if (search != ref_sequences.end()){
        seqID = search->second;
        return true;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Reader::Reader(std::istream& _stream, Constraint _constraint, uint64_t _constraint_val):
    stream(_stream), header(stream), rec_saved(false), constraint(_constraint), constraint_val(_constraint_val), num_records(0){

    uint16_t seqID = 0;
    for (auto &headerLine : header.getLines()){
        if (headerLine.getName() == "SQ") {
            for (auto &tag : headerLine.getTags()) {
                auto tagName = tag->getName();

                if (tagName == "SN"){
                    ref_sequences.emplace(tag->toString(), seqID);
                } else if(tagName == "AN"){
                    // TODO : value of tag AN is comma-separated list (yeremia)
                    ref_sequences.emplace(tag->toString(), seqID);
                }
            }
            seqID++;
        }
    }

#if WITH_CACHE
    int init_pos = stream.tellg();

    while(stream.peek() != EOF){
        std::string str;
        std::getline(stream, str, '\t');

        if (str[0] != '@'){
            size_t pos = stream.tellg();

            addCacheEntry(str, pos);

            std::getline(stream, str);
        }

    }

//    while(std::getline(stream, str)){
//        auto qname = str.substr(0, str.find('\t'));
//
//        pos = stream.tellg();
//    }

    stream.clear();
    stream.seekg(init_pos);

    UTILS_DIE_IF(stream.tellg() != init_pos, "Unable to move file pointer");
#else

#endif
}

// ---------------------------------------------------------------------------------------------------------------------

const header::Header& Reader::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::isConstrainReached() const {
    switch (constraint){
        case Constraint::BY_NUM_RECORDS :
            return num_records > constraint_val;
        // TODO: Constraint BY_SIZE (Yeremia)
        case Constraint::BY_SIZE :
            UTILS_DIE(" Not yet implemented : Constraint BY_SIZE");
        case Constraint::NONE:
            return false;
        default:
            UTILS_DIE("Invalid value of constraint / constraint mode not yet implemented");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Reader::addRecord(Record& record) {
    // TODO: Store values for other constraints such as BY_SIZE (Yeremia)
    num_records++;

    if (!record.isUnmapped()){
        UTILS_DIE_IF(!isReferenceExists(record.getRname()), "No reference in header");
    }

    const auto& qname = record.getQname();

    auto entry = data.find(qname);
    if (entry == data.end()) {
        data.emplace(qname, std::vector<std::vector<Record>>());

        for (uint8_t i = 0; i < uint8_t(Index::TOTAL_TYPES); i++) {
            data[qname].emplace_back(std::vector<Record>());
        }
    }

    Reader::Index idx;

    // Paired-end read / multi segments
    if (record.checkFlag(Record::FlagPos::MULTI_SEGMENT_TEMPLATE)) {
        if (record.checkFlag(Record::FlagPos::FIRST_SEGMENT)) {
            idx = record.isPrimaryLine() ? Index::PAIR_FIRST_PRIMARY : Index::PAIR_FIRST_NONPRIMARY;
        } else if (record.checkFlag(Record::FlagPos::LAST_SEGMENT)) {
            idx = record.isPrimaryLine() ? Index::PAIR_LAST_PRIMARY : Index::PAIR_LAST_NONPRIMARY;
        } else {
            UTILS_DIE("Unknown type of template found");
        }
        // Single-end read / single segment
    } else if (!record.checkFlag(Record::FlagPos::FIRST_SEGMENT) && !record.checkFlag(Record::FlagPos::LAST_SEGMENT)) {
        idx = record.checkFlag(Record::FlagPos::SEGMENT_UNMAPPED) ? Index::SINGLE_UNMAPPED : Index::SINGLE_MAPPED;
    } else {
        UTILS_DIE("Unknown type of template found");
    }

    data[qname][uint8_t(idx)].push_back(std::move(record));
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO: Prevent over-/underflow (Yeremia)
void Reader::read(int num) {
    std::string str;

#if WITH_CACHE
    for (auto iter = cache.begin(); iter != cache.end(); iter++){
        for (auto& pos : iter->second){
            stream.seekg(pos);
            std::getline(stream, str);

            Record rec(iter->first, str);
            addRecord(rec);

            num--;
        }

        cache.erase(iter);

        if (num <= 0) {
            break;
        }
    }
#else
    while (num && std::getline(stream, str)) {
        addRecord(str);
        num--;
    }
#endif

}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::getSortedTemplate(std::list<Record>& unmappedRead, std::list<Record>& read1, std::list<Record>& read2) {

    unmappedRead.clear();
    read1.clear();
    read2.clear();

    if (num_records) {
        for (auto& iter : data) {

            auto isSingleEndUnmapped = !(iter.second[uint8_t(Index::SINGLE_UNMAPPED)].empty());
            auto isSingleEndMapped = !iter.second[uint8_t(Index::SINGLE_MAPPED)].empty();
            auto isPairedEndPrimary = !(iter.second[uint8_t(Index::PAIR_FIRST_PRIMARY)].empty() &&
                                  iter.second[uint8_t(Index::PAIR_LAST_PRIMARY)].empty());
            auto isPairedEndNonPrimary = !(iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].empty() &&
                                  iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].empty());

            // Check if template is either single or paired-end using xor operation
            UTILS_DIE_IF(!(((isSingleEndUnmapped || isSingleEndMapped) || (isPairedEndPrimary || isPairedEndNonPrimary)) &&
                           !((isSingleEndUnmapped || isSingleEndMapped) && (isPairedEndPrimary || isPairedEndNonPrimary))),
                         "Template with both single- and paired-end reads found");

            UTILS_DIE_IF(isSingleEndUnmapped && isSingleEndMapped, "Invalid single-ended read");

            if (isSingleEndUnmapped) {
                num_records -= iter.second[uint8_t(Index::SINGLE_UNMAPPED)].size();
                std::move(iter.second[uint8_t(Index::SINGLE_UNMAPPED)].begin(),
                          iter.second[uint8_t(Index::SINGLE_UNMAPPED)].end(),
                          std::back_inserter(unmappedRead));

            } else if (isSingleEndMapped) {
                num_records -= iter.second[uint8_t(Index::SINGLE_MAPPED)].size();

                std::move(iter.second[uint8_t(Index::SINGLE_MAPPED)].begin(),
                          iter.second[uint8_t(Index::SINGLE_MAPPED)].end(),
                          std::back_inserter(read1));

            } else if (isPairedEndPrimary) {
                UTILS_DIE_IF(
                        iter.second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].size() != iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].size(),
                        "Number of alignment for first and last segment must be equal!");

                num_records -= iter.second[uint8_t(Index::PAIR_FIRST_PRIMARY)].size();
                num_records -= iter.second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].size();
                num_records -= iter.second[uint8_t(Index::PAIR_LAST_PRIMARY)].size();
                num_records -= iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].size();

                std::move(iter.second[uint8_t(Index::PAIR_FIRST_PRIMARY)].begin(),
                          iter.second[uint8_t(Index::PAIR_FIRST_PRIMARY)].end(),
                          std::back_inserter(read1));

                std::move(iter.second[uint8_t(Index::PAIR_LAST_PRIMARY)].begin(),
                          iter.second[uint8_t(Index::PAIR_LAST_PRIMARY)].end(),
                          std::back_inserter(read2));

                // Without pair sorting
//                std::move(iter.second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].begin(),
//                          iter.second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].end(),
//                          std::back_inserter(read1));
//
//                std::move(iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].begin(),
//                          iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].end(),
//                          std::back_inserter(read2));

                while (!iter.second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].empty()){

                    bool pairFound = false;
                    auto iter_r1 = iter.second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].begin();
                    auto iter_r2 = iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].begin();

                    while (iter_r2 != iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].end()){
                        if (iter_r1->isPairOf(*iter_r2)){
                            pairFound = true;

                            // TODO: Split alignment (Yeremia)
                            UTILS_DIE_IF(iter_r1->getRname() != iter_r2->getRname(),
                                         "Not yet implemented : Split alignment for SAM record");

                            read1.push_back(std::move(*iter_r1));
                            read2.push_back(std::move(*iter_r2));

                            iter.second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].erase(iter_r1);
                            iter.second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].erase(iter_r2);
                            break;
                        } else {
                            iter_r2++;
                        }
                    }
                    UTILS_DIE_IF(!pairFound, "No pair for alignment found!");
                }
            } else {
#if WITH_CACHE
                UTILS_DIE("Invalid alignment with QNAME " + iter.first);
#else
                continue;
#endif
            }
            data.erase(iter.first);
            return true;
        }
    } else {
        return false;
    }

}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::isEnd() { return stream.eof(); }

const std::map<std::string, std::vector<std::vector<Record>>>& Reader::getData() const {
    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------