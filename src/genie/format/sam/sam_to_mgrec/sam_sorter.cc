#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"

#include <fcntl.h>
#include <genie/util/runtime-exception.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <mutex>
#include <ostream>
#include <thread>


namespace genie::format::sam::sam_to_mgrec {


/** compare function for open reads **/
bool CmpOpen::operator()(std::vector<sam_to_mgrec::SamRecord>& a, std::vector<sam_to_mgrec::SamRecord>& b) const {
    UTILS_DIE_IF(a.empty() || b.empty(),"emtpy sam read in open list");
    if (a.at(a.size() -1).mate_pos == b.at(b.size() -1).mate_pos) {
        return a.at(a.size() -1).qname > b.at(b.size() -1).qname;
    }
    return a.at(a.size() -1).mate_pos > b.at(b.size() -1).mate_pos;
}

// ---------------------------------------------------------------------------------------------------------------------

/** compare function for compete reads **/
bool CmpCompete::operator()(std::vector<sam_to_mgrec::SamRecord>& a, std::vector<sam_to_mgrec::SamRecord>& b) const {
    UTILS_DIE_IF(a.empty() || b.empty(),"emtpy sam read in closed list");
    return a.at(0).pos > b.at(0).pos;
}

// ---------------------------------------------------------------------------------------------------------------------

SamSorter::SamSorter(std::string& file_path) :
    sam_file_path(std::move(file_path)),
    sam_reader(genie::format::sam::sam_to_mgrec::SamReader(sam_file_path)),
    open_queries(CmpOpen()),
    completed_queries(CmpCompete()) {
    UTILS_DIE_IF(!sam_reader.isReady(), "Cannot open SAM file.");
}

// ---------------------------------------------------------------------------------------------------------------------

/** gets the smallest primary alignment position of the open list or maximum **/
void SamSorter::set_new_alignment_pos() {
    if (!open_queries.empty()) {
        UTILS_DIE_IF(primary_alignment_positions.size() > open_queries.size(), "too many positions");
        UTILS_DIE_IF(primary_alignment_positions.size() > open_queries.size(), "not enough positions");
        cur_alignment_position = *std::min_element(primary_alignment_positions.begin(), primary_alignment_positions.end());
    } else {
        cur_alignment_position = INT64_MAX;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SamSorter::try_appending_to_queries(std::vector<SamRecord> cur_query, std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries) {
    /** cur_query is complete, try appending to queries **/
    if (cur_alignment_position >= cur_query.at(0).pos) {
        /** allowed to append to queries **/
        queries.emplace_back();
        queries.back() =  cur_query;
    } else {
        /** not allowed to append to queries, append to complete_reads **/
        completed_queries.push(cur_query);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool SamSorter::isReadComplete(std::vector<sam_to_mgrec::SamRecord> read) {
    return read.at(read.size()-1).mate_pos == read.at(0).pos;
}

// ---------------------------------------------------------------------------------------------------------------------

void SamSorter::addSamRead_thread(std::mutex& lock, std::vector<std::vector<sam_to_mgrec::SamRecord>>& data) {
    int ret = 0;

    // reading data from sam file
    {
        std::lock_guard<std::mutex> guard(lock);
        std::cerr << "reading file" << std::endl;
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            data.emplace_back();
            ret = sam_reader.readSamQuery(data.back());
            if (ret == EOF) {
                if (data.back().empty()) {
                    data.pop_back();
                }
                break;
            }
            UTILS_DIE_IF(ret, "Error reading sam query: " + std::string(strerror(ret)));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SamSorter::addSamRead(std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries, uint32_t num_threads) {
    // get new data from sam reader
    std::mutex lock;
    std::vector<std::thread> threads;
    std::vector<std::vector<sam_to_mgrec::SamRecord>> data;
    threads.reserve(num_threads);
    for (uint32_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(
            [&]() { addSamRead_thread(lock, data); });
    }
    for (auto& t : threads) {
        t.join();
    }

    /** TODO: Sometimes there are multiple open queries that wait for the same mate position. When top returns the "wrong" one of them, they can't be matched. */

    // sorting data
    std::cerr << "start with sorting data" << std::endl;

    for (std::vector<sam_to_mgrec::SamRecord> &query : data) {
        for (sam_to_mgrec::SamRecord &record: query) {

            /** is it still okay to wait for top element? **/
            while (!open_queries.empty() && record.pos >= open_queries.top().at(open_queries.top().size() - 1).mate_pos &&
                record.qname != open_queries.top().at(0).qname){
                /** couldn't finde mate of open_queries.top() **/
                queries.emplace_back();
                queries.back() =  open_queries.top();
                primary_alignment_positions.erase(primary_alignment_positions.begin());
                open_queries.pop();
            }

            /** check if read does not belong to existing query **/
            if (open_queries.empty() || record.qname != open_queries.top().at(0).qname) {
                if (std::vector<sam_to_mgrec::SamRecord> cur_query = {record}; isReadComplete(cur_query)) {
                    try_appending_to_queries(cur_query, queries);
                } else {
                    /** cur_query is not complete yet, put to open_queries again **/
                    open_queries.push(cur_query);
                    primary_alignment_positions.push_back(record.pos);
                }
            } else {
                /** record belongs to an already existing query **/
                std::vector<SamRecord> cur_query = open_queries.top();
                open_queries.pop();
                cur_query.push_back(record);
                if (isReadComplete(cur_query)) {
                    try_appending_to_queries(cur_query, queries);
                    primary_alignment_positions.erase(primary_alignment_positions.begin());
                } else {
                    /** query is still not complete **/
                    open_queries.push(cur_query);
                }
            }

            set_new_alignment_pos();

            /** check if any completed queries can be put to queries **/
            while (!completed_queries.empty() && cur_alignment_position >= completed_queries.top().at(0).pos) {
                queries.emplace_back();
                queries.back() = completed_queries.top();
                completed_queries.pop();
            }
        }
    }

    std::cerr << "sorting complete" << std::endl;
}

}  // namespace genie::format::sam::sam_to_mgrec

//