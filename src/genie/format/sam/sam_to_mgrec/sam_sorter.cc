//
// Created by sophie on 04.12.24.
//

#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"

#include <genie/util/runtime-exception.h>

#include <iostream>
#include <mutex>
#include <ostream>
#include <thread>


namespace genie::format::sam::sam_to_mgrec {


SamSorter::SamSorter(std::string& file_path):
    sam_file_path(std::move(file_path)),
    sam_reader(genie::format::sam::sam_to_mgrec::SamReader(file_path)),
    cur_alignment_position(0) {
    std::cerr << "SamSorter: " << sam_file_path << std::endl;
}


void addSamRead_thread(std::mutex& lock, SamReader& sam_reader, std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries) {
    std::vector<std::vector<sam_to_mgrec::SamRecord>> data;
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

    // sorting data
    queries = data;
}


void SamSorter::addSamRead(std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries, uint32_t num_threads) {
    // get new data from sam reader
    std::mutex lock;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (uint32_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(
            [&]() { addSamRead_thread(lock, sam_reader, queries); });
    }
    for (auto& t : threads) {
        t.join();
    }
}

bool isReadComplete(std::vector<sam_to_mgrec::SamRecord>& read);

}  // namespace genie::format::sam::sam_to_mgrec