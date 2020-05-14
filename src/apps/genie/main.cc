#include <genie/module/manager.h>
#include <genie/run/main.h>
#include <genie/util/exceptions.h>
#include <cli11@13becad/CLI11.hpp>
#include <iostream>
#include <string>
#include <vector>

int stat (int , char* []) {
    UTILS_DIE("Stat not implemented");
}

int help (int , char* []) {
    UTILS_DIE("Help text not helpfull enough");
}

int main (int argc, char* argv[]) {
#ifdef GENIE_USE_OPENMP
    std::cout << "genie: built with OpenMP\n\n" << std::endl;
#else
    std::cout << "genie: *not* built with OpenMP\n\n" << std::endl;
#endif
    genie::module::detect();
    constexpr int OPERATION_INDEX = 1;
    try {
        UTILS_DIE_IF(argc <= OPERATION_INDEX, "No operation specified, type 'genie help' for more info.");
        std::string operation = argv[OPERATION_INDEX];
        transform(operation.begin(), operation.end(), operation.begin(), ::tolower);
        if(operation == "run") {
            genieapp::run::main(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
        } else if (operation == "stat") {
            stat(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
        } else if (operation == "help") {
            help(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
        } else {
            UTILS_DIE("Unknown operation " + operation);
        }

    } catch (const genie::util::Exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}