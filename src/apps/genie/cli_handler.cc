/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "apps/genie/cli_handler.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "cli11/CLI11_RESTORED.hpp"

namespace genieapp::cli {

CliOptions parse_args(int argc, char* argv[]) {
    CliOptions options;
    if (argc < 2) {
        return options;
    }

    options.operation = argv[1];
    std::transform(options.operation.begin(), options.operation.end(), options.operation.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    for (int i = 2; i < argc; ++i) {
        options.args.push_back(argv[i]);
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            options.help_requested = true;
        }
    }

    return options;
}

}  // namespace genieapp::cli
