//
// Created by sophie on 04.12.24.
//

#include "apps/genie/sam_sort_test/main.h"

#include <format/sam/sam_to_mgrec/sam_reader.h>
#include <util/runtime-exception.h>

#include <iostream>
#include <mutex>
#include <ostream>
#include <queue>
#include <thread>

#include "format/sam/sam_to_mgrec/sam_sorter.h"

namespace genieapp::sam_sort_test {

void get_sam_file(std::string& file_path, std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>>& queries) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter = genie::format::sam::sam_to_mgrec::SamSorter(file_path);
    uint32_t num_threads = std::thread::hardware_concurrency();
    sam_sorter.addSamRead(queries,num_threads);

}

int main(int argc, char** argv) {
    std::cerr << "test sorter started" << std::endl;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> queries;

    if (argc <= 1) {
        std::cerr << "input sam file required" << std::endl;
    } else {
        std::string input_file_path = argv[1];
        std::cerr << "input sam file provided: " << input_file_path << std::endl;
        get_sam_file(input_file_path, queries);
    }
    return 0;
}

} // genieapp::sam_sort_test