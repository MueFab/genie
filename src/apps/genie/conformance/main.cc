/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/conformance/program-options.h"
#include <iostream>
#include <fstream>
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/format/mpegg_p1/mpegg_file.h"
//#include "genie/format/mpegg_p1/file_header.h"
//#include "genie/format/mpegg_p1/dataset_group.h"
#include "genie/format/mgb/raw_reference.h"
//#include "genie/format/mpegg_p1/dataset_group.h"

#include <cstdint>
#include "genie/format/mpegg_p1/reference/reference_location/external.h"

namespace genieapp {
namespace conformance {

enum class ErrorCode : uint8_t { success = 0, failure = 1 };
enum class DataUnitType : uint8_t { RAW_REF = 0, PAR_SET = 1, ACC_UNT = 2};

ErrorCode test(ProgramOptions& options){

    for (auto i=5;i<27;i++){
        auto fpath = options.getFilePath(i);

        std::ifstream reader(fpath.string());
        genie::util::BitReader bitreader(reader);

        genie::format::mpegg_p1::MpeggFile mgg_file(bitreader);

        std::cout << "Checking " << options.getFilePath(i) << "..." << std::endl;

        switch (i){
            case 1: {
                auto& ds_groups = mgg_file.getDatasetGroups();

                auto& dss = ds_groups[0].getDatasets();
                auto ds_ID = dss[0].getID();
                UTILS_DIE_IF(ds_ID != 2,
                             "Wrong dataset_ID found!");

                auto& refs = ds_groups[0].getReferences();
                auto& ref_loc = refs[0].getReferenceLocation();

                UTILS_DIE_IF(!ref_loc.isExternal(),
                             "No external reference found!");

                auto& ext_loc = dynamic_cast<const genie::format::mpegg_p1::External&>(ref_loc);
                auto& ext_ref = ext_loc.getExternalRef();
                auto type = ext_ref.getType();

                UTILS_DIE_IF(type != genie::format::mpegg_p1::ExternalReference::Type::FASTA_REF,
                             "Wrong external reference type found!");
                break;
            }
            case 2: {
                continue;
                auto& ds_groups = mgg_file.getDatasetGroups();

                auto& dss = ds_groups[0].getDatasets();
                auto ds_ID = dss[0].getID();
                UTILS_DIE_IF(ds_ID != 2,
                             "Wrong dataset_ID found!");
                break;
            }
            case 3: {
                break;
            }
            case 4: {
                break;
            }
            case 5: {
                break;
            }
            case 6: {
                break;
            }
            case 7: {
                break;
            }
            case 8: {
                break;
            }
            default:
                UTILS_DIE("Invalid conformance test item!");
        }
    }

    return ErrorCode::success;
}

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    genieapp::conformance::ErrorCode ret = genieapp::conformance::ErrorCode::success;

    ret = test(pOpts);

    return (int) ret;
}

}
}