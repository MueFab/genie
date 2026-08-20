/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_CLI_HANDLER_H_
#define SRC_APPS_GENIE_CLI_HANDLER_H_

#include <string>
#include <vector>

namespace genieapp::cli {

struct CliOptions {
    std::string operation;
    std::vector<std::string> args;
    bool help_requested = false;
};

CliOptions parse_args(int argc, char* argv[]);

}  // namespace genieapp::cli

#endif  // SRC_APPS_GENIE_CLI_HANDLER_H_
