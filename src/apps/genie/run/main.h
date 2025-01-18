/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_RUN_MAIN_H_
#define SRC_APPS_GENIE_RUN_MAIN_H_
#include <string>

// -----------------------------------------------------------------------------

namespace genie_app::run {

std::string file_extension(const std::string& path);

/**
 * @brief
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]);

// -----------------------------------------------------------------------------

}  // namespace genie_app::run

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_RUN_MAIN_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
