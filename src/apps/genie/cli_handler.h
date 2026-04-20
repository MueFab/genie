#ifndef GENIE_APPS_GENIE_CLI_HANDLER_H
#define GENIE_APPS_GENIE_CLI_HANDLER_H

#include <string>
#include <vector>

namespace genieapp::cli {

struct CliOptions {
    std::string operation;
    std::vector<std::string> args;
    bool help_requested = false;
};

CliOptions parse_args(int argc, char* argv[]);

} // namespace genieapp::cli

#endif
