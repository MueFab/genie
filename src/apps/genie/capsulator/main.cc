/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/capsulator/program-options.h"
#include <iostream>
#include <fstream>
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/util.h"
//#include "genie/format/mpegg_p1/dataset_group.h"

namespace genieapp {
namespace capsulator {

enum class ErrorCode : uint8_t { success = 0, failure = 1 };
enum class DataUnitType : uint8_t { RAW_REF = 0, PAR_SET = 1, ACC_UNT = 2};

ErrorCode encapsulate(ProgramOptions& options){

}

ErrorCode decapsulate(ProgramOptions& options){

    std::ifstream reader(options.inputFile);
    genie::util::BitReader bitreader(reader);

    std::string key = genie::format::mpegg_p1::readKey(bitreader);
    if (key == "flhl"){

    } else if (key == "dgcn") {
        DataUnitType data_unit_type = bitreader.read<DataUnitType>();
        uint64_t data_unit_size;
        if (data_unit_type == DataUnitType::RAW_REF){
            data_unit_size = bitreader.read<uint64_t>();
            std::cout << "OK";
        } else if (data_unit_type == DataUnitType::PAR_SET) {
            bitreader.read<uint64_t>(10);
            data_unit_size = bitreader.read<uint64_t>(22);
            std::cout << "OK";
        } else if (data_unit_type == DataUnitType::ACC_UNT) {
            bitreader.read<uint64_t>(3);
            data_unit_size = bitreader.read<uint64_t>(29);
            std::cout << "OK";
        } else {
            UTILS_DIE("Invalid data unit type found");
        }
    } else {
        UTILS_DIE("Invalid box");
    }


}

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
}

}
}