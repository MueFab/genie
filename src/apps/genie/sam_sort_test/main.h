/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_SAM_SORT_TEST_MAIN_H
#define SRC_APPS_GENIE_SAM_SORT_TEST_MAIN_H

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/format/sam/sam_to_mgrec/sam_record.h"

// -----------------------------------------------------------------------------

namespace genieapp::sam_sort_test {

// -----------------------------------------------------------------------------

std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>>
get_sam_file(const std::string& file_path);

/**
 * @brief
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]);

// -----------------------------------------------------------------------------

}  // namespace genieapp::sam_sort_test

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_SAM_SORT_TEST_MAIN_H

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
