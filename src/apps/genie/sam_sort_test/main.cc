#define BUFFER_SIZE 1000000
#include "apps/genie/sam_sort_test/main.h"

#include <format/sam/sam_to_mgrec/sam_reader.h>
#include <util/runtime-exception.h>

#include <iostream>
#include <mutex>
#include <ostream>
#include <queue>
#include <random>
#include <thread>

#include "format/sam/sam_to_mgrec/sam_sorter.h"

namespace genieapp::sam_sort_test {

void get_samfile_thread(std::mutex& lock_read, std::mutex& lock_sort, std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>>& queries,
    genie::format::sam::sam_to_mgrec::SamReader& sam_reader, genie::format::sam::sam_to_mgrec::SamSorter sam_sorter) {
    int ret = 0;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> data;
    // reading data from sam file
    {
        std::lock_guard<std::mutex> guard(lock_read);
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
    {
        std::lock_guard<std::mutex> guard(lock_sort);
        std::cerr << "sorting file" << std::endl;
        for (auto& query : data) {
            for (auto& record : query) {
                sam_sorter.addSamRead(record);
            }
        }
        std::cerr << "getting queries" << std::endl;
        data = sam_sorter.getQueries();
        queries.insert(queries.end(), data.begin(), data.end());
        std::cerr << "done" << std::endl;
    }
}

int main(int argc, char** argv) {
    std::cerr << "test sorter started" << std::endl;

    if (argc <= 1) {
        std::cerr << "input sam file required" << std::endl;
        return 0;
    }
    std::string input_file_path = argv[1];
    std::cerr << "input sam file provided: " << input_file_path << std::endl;
    genie::format::sam::sam_to_mgrec::SamReader sam_reader =  genie::format::sam::sam_to_mgrec::SamReader(input_file_path);
    UTILS_DIE_IF(!sam_reader.isReady(), "Cannot open SAM file.");
    int num_threads = std::thread::hardware_concurrency();

    // here starts the reading of the file
    std::mutex lock_read;
    std::mutex lock_sort;
    std::vector<std::thread> threads;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> queries;
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter = genie::format::sam::sam_to_mgrec::SamSorter(100000);
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(
        [&]() { get_samfile_thread(lock_read, lock_sort, queries, sam_reader, sam_sorter); });
    }
    for (auto& t : threads) {
        t.join();
    }

    int count = 0;
    for (auto& query : queries) {
        std::cerr << query.size() << std::endl;
    }
    std::cerr << count << std::endl;
    return 0;
}

} // genieapp::sam_sort_test