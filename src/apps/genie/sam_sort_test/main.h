//
// Created by sophie on 04.12.24.
//

#ifndef SRC_APPS_GENIE_SAM_SORT_TEST_MAIN_H
#define SRC_APPS_GENIE_SAM_SORT_TEST_MAIN_H
#include <format/sam/sam_to_mgrec/sam_record.h>

#include <string>
#include <vector>

namespace genieapp::sam_sort_test {

std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> get_sam_file(const std::string& file_path);

/**
 * @brief
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]);

} // genieapp::sam_sort_test

#endif //SRC_APPS_GENIE_SAM_SORT_TEST_MAIN_H
