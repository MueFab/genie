/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/capsulator/main.h"
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "apps/genie/capsulator/program-options.h"
#include "filesystem/filesystem.hpp"
#include "format/mgb/mgb_file.h"
#include "format/mgg/encapsulator/decapsulated_file.h"
#include "format/mgg/encapsulator/encapsulated_file.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mgg/mgg_file.h"
#include "genie/util/runtime-exception.h"
#include "util/string-helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace capsulator {

// ---------------------------------------------------------------------------------------------------------------------

void encapsulate(ProgramOptions& options) {
    auto version = genie::core::MPEGMinorVersion::V2000;

    //   mgb_file.remove_class(genie::core::record::ClassType::CLASS_N);
    //   mgb_file.remove_class(genie::core::record::ClassType::CLASS_M);
    //   mgb_file.select_mapping_range(0, 54111088, 101380838);
    //   mgb_file.sort_by_class();

    // mgb_file.print_debug(std::cout, 100);

    auto inputs = genie::util::tokenize(options.inputFile, ';');
    auto input_file = genie::format::mgg::encapsulator::EncapsulatedFile(inputs, version);
    auto mgg_file = input_file.assemble(version);

    std::ofstream outstream(options.outputFile);
    genie::util::BitWriter writer(&outstream);

    mgg_file.print_debug(std::cerr, 100);

    mgg_file.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void decapsulate(ProgramOptions& options) {
    genie::format::mgg::encapsulator::DecapsulatedFile ret(options.inputFile);
    std::string global_output_prefix = options.outputFile.substr(0, options.outputFile.find_last_of('.'));

    for (auto& grp : ret.getGroups()) {
        for (auto& dt : grp.getData()) {
            std::string local_output_prefix =
                global_output_prefix + "." + std::to_string(grp.getID()) + "." + std::to_string(dt.first);
            auto meta_json = dt.second.second.toJson().dump(4);
            std::ofstream json_file(local_output_prefix + ".mgb.json");
            json_file.write(meta_json.data(), meta_json.length());

            std::ofstream mgb_output_file(local_output_prefix + ".mgb");
            genie::util::BitWriter mgb_output_writer(&mgb_output_file);
            dt.second.first.write(mgb_output_writer);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    try {
        if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgb" &&
            pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgg") {
            encapsulate(pOpts);
        } else if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgg" &&
                   pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgb") {
            decapsulate(pOpts);
        }
    } catch (genie::util::RuntimeException& e) {
        std::cerr << e.msg() << std::endl;
        return -1;
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return -1;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace capsulator
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
