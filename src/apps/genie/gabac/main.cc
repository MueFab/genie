/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/gabac/main.h"
#include <cassert>
#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "apps/genie/gabac/code.h"
#include "apps/genie/gabac/program-options.h"
#include "benchmark.h"
#include "genie/entropy/gabac/gabac.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    try {
        ProgramOptions programOptions(argc, argv);

        if (programOptions.task == "encode") {
            code(programOptions.inputFilePath, programOptions.outputFilePath, programOptions.paramFilePath,
                 programOptions.blocksize, programOptions.descID, programOptions.subseqID, false,
                 programOptions.dependencyFilePath);
        } else if (programOptions.task == "decode") {
            code(programOptions.inputFilePath, programOptions.outputFilePath, programOptions.paramFilePath,
                 programOptions.blocksize, programOptions.descID, programOptions.subseqID, true,
                 programOptions.dependencyFilePath);
        } else if (programOptions.task == "writeconfigs") {
            for (const auto& d : genie::core::getDescriptors()) {
                for (const auto& s : d.subseqs) {
                    auto conf = genie::entropy::gabac::EncodingConfiguration(
                                    genie::entropy::gabac::getEncoderConfigManual(s.id))
                                    .toJson()
                                    .dump(4);
                    std::ofstream outstream("gabacconf_" + std::to_string(static_cast<uint8_t>(s.id.first)) + "_" +
                                            std::to_string(s.id.second) + ".json");
                    outstream << conf;
                }
            }
        } else if (programOptions.task == "benchmark") {
            float timeweight = 0.0f;
            if (programOptions.fastBenchmark) {
                timeweight = 1.0f;
            }
            auto result = genie::entropy::gabac::benchmark_full(
                programOptions.inputFilePath,
                genie::core::GenSubIndex(
                    std::make_pair(genie::core::GenDesc(programOptions.descID), programOptions.subseqID)),
                timeweight);
            auto json = result.config.toJoson().dump(4);
            std::ofstream output_stream(programOptions.outputFilePath);
            output_stream.write(json.c_str(), json.length());

        } else {
            UTILS_DIE("Invalid task: " + std::string(programOptions.task));
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unkown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
