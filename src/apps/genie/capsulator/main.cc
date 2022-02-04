/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/capsulator/main.h"
#include <fstream>
#include <iostream>
#include "apps/genie/capsulator/program-options.h"
#include "format/mgb/mgb_file.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mpegg_p1/mgg_file.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace capsulator {

// ---------------------------------------------------------------------------------------------------------------------

enum class ErrorCode : uint8_t { success = 0, failure = 1 };
enum class DataUnitType : uint8_t { RAW_REF = 0, PAR_SET = 1, ACC_UNT = 2 };

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode encapsulate(ProgramOptions& options) {
    auto version = genie::core::MPEGMinorVersion::V2000;

    std::ifstream reader(options.inputFile);

    genie::format::mgb::MgbFile mgb_file(&reader);

    //   mgb_file.remove_class(genie::core::record::ClassType::CLASS_N);
    //   mgb_file.remove_class(genie::core::record::ClassType::CLASS_M);
    //   mgb_file.select_mapping_range(0, 54111088, 101380838);
    //   mgb_file.sort_by_class();

    // mgb_file.print_debug(std::cout, 100);

    genie::core::meta::Dataset meta;

    genie::format::mpegg_p1::MggFile mpegg_file;
    mpegg_file.addBox(genie::util::make_unique<genie::format::mpegg_p1::FileHeader>(version));
    auto group = genie::util::make_unique<genie::format::mpegg_p1::DatasetGroup>(0, 0, version);

    for (uint8_t multiple_alignment = 0; multiple_alignment < 2; ++multiple_alignment) {
        for (uint8_t pos40 = 0; pos40 < 2; ++pos40) {
            for (uint8_t data_type = 0; data_type < uint8_t(genie::core::parameter::DataUnit::DatasetType::COUNT);
                 ++data_type) {
                for (uint8_t alphabet_type = 0; alphabet_type < uint8_t(genie::core::AlphabetID::COUNT);
                     ++alphabet_type) {
                    auto param_ids = mgb_file.collect_param_ids(
                        bool(multiple_alignment), bool(pos40), genie::core::parameter::DataUnit::DatasetType(data_type),
                        genie::core::AlphabetID(alphabet_type));
                    if (param_ids.empty()) {
                        continue;
                    }
                    auto dataset = genie::format::mpegg_p1::Dataset(mgb_file, meta, version, param_ids);
                    group->addDataset(std::move(dataset));
                }
            }
        }
    }

    mpegg_file.addBox(std::move(group));

    std::ofstream outstream(options.outputFile);
    genie::util::BitWriter writer(&outstream);

    mpegg_file.print_debug(std::cout, 100);

    mpegg_file.write(writer);

    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode decapsulate(ProgramOptions& options) {
    std::ifstream reader(options.inputFile);

    genie::format::mpegg_p1::MggFile mpegg_file(&reader);
    mpegg_file.print_debug(std::cout, 2);

    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    genieapp::capsulator::ErrorCode ret = genieapp::capsulator::ErrorCode::success;
    /// Encapsulate
    if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgb" &&
        pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgg") {
        ret = encapsulate(pOpts);
        /// Decapsulate
    } else if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgg" &&
               pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgb") {
        ret = decapsulate(pOpts);
    }
    if (ret != genieapp::capsulator::ErrorCode::success) {
        UTILS_DIE("ERROR");
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace capsulator
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
