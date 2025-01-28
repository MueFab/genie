/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define BUFFER_SIZE 1000000
#include "apps/genie/sam_sort_test/main.h"

#include <iostream>
#include <mutex>
#include <ostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "format/sam/sam_to_mgrec/sam_sorter.h"
#include "genie/format/sam/sam_to_mgrec/sam_reader.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genieapp::sam_sort_test {

// -----------------------------------------------------------------------------

void get_samfile_thread(
    std::mutex& lock_read, std::mutex& lock_sort,
    std::vector<std::vector<genie::format::sam::SamRecord>>&
        queries,
    genie::format::sam::sam_to_mgrec::SamReader& sam_reader,
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter) {
  std::vector<std::vector<genie::format::sam::SamRecord>> data;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecordPair> pairs;
  // reading data from sam file
  {
    int ret = 0;
    std::lock_guard guard(lock_read);
    std::cerr << "reading file" << std::endl;
    for (int i = 0; i < BUFFER_SIZE; ++i) {
      data.emplace_back();
      ret = sam_reader.ReadSamQuery(data.back());
      if (ret == EOF) {
        if (data.back().empty()) {
          data.pop_back();
        }
        break;
      }
      UTILS_DIE_IF(ret,
                   "Error reading sam query: " + std::string(strerror(ret)));
    }
  }
  {
    std::lock_guard guard(lock_sort);
    std::cerr << "sorting file" << std::endl;
    for (auto& query : data) {
      for (auto& record : query) {
        sam_sorter.AddSamRead(record);
      }
    }
    std::cerr << "getting queries" << std::endl;
    pairs = sam_sorter.GetPairs();
    queries.insert(queries.end(), data.begin(), data.end());
    std::cerr << "done" << std::endl;
  }
}

// -----------------------------------------------------------------------------

int main(int argc, char** argv) {
  std::cerr << "test sorter started" << std::endl;

  if (argc <= 1) {
    std::cerr << "input sam file required" << std::endl;
    return 0;
  }
  std::string input_file_path = argv[1];
  std::cerr << "input sam file provided: " << input_file_path << std::endl;
  auto sam_reader =
      genie::format::sam::sam_to_mgrec::SamReader(input_file_path);
  UTILS_DIE_IF(!sam_reader.IsReady(), "Cannot open SAM file.");

  // here starts the reading of the file
  std::mutex lock_read;
  std::mutex lock_sort;
  std::vector<std::thread> threads;
  std::vector<std::vector<genie::format::sam::SamRecord>> queries;
  auto sam_sorter = genie::format::sam::sam_to_mgrec::SamSorter(100000);
  threads.reserve(1);
  for (int i = 0; i < 1; ++i) {
    threads.emplace_back([&] {
      get_samfile_thread(lock_read, lock_sort, queries, sam_reader, sam_sorter);
    });
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

// -----------------------------------------------------------------------------

}  // namespace genieapp::sam_sort_test

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
