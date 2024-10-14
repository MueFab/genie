/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_RUN_MAIN_H_
#define SRC_APPS_GENIE_RUN_MAIN_H_
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::run {

std::string file_extension(const std::string& path);

/**
 * @brief
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::run

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_RUN_MAIN_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
