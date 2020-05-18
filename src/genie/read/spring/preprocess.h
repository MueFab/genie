/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_PREPROCESS_H_
#define SPRING_PREPROCESS_H_

#include <filesystem@e980ed0/filesystem.hpp>
#include <string>
#include "spring.h"
#include "util.h"

#include <genie/core/record/chunk.h>

namespace genie {
namespace read {
namespace spring {

struct Preprocessor {
    compression_params cp;

    std::string outfileclean[2];
    std::string outfileN[2];
    std::string outfileorderN[2];
    std::string outfileid;
    std::string outfilequality[2];
    std::string outfilereadlength[2];

    std::ofstream fout_clean[2];
    std::ofstream fout_N[2];
    std::ofstream fout_order_N[2];
    std::ofstream fout_id;
    std::ofstream fout_quality[2];

    std::string temp_dir;

    bool init = false;

    void setup(bool preserveID, bool preserveQV, const std::string& working_dir) {
        cp.preserve_id = preserveID;
        cp.preserve_quality = preserveQV;
        // generate random temp directory in the working directory

        while (true) {
            std::string random_str = "tmp." + spring::random_string(10);
            temp_dir = working_dir + "/" + random_str + '/';
            if (!ghc::filesystem::exists(temp_dir)) break;
        }
        UTILS_DIE_IF(!ghc::filesystem::create_directory(temp_dir), "Cannot create temporary directory.");
        std::cout << "Temporary directory: " << temp_dir << "\n";

        outfileclean[0] = temp_dir + "/input_clean_1.dna";
        outfileclean[1] = temp_dir + "/input_clean_2.dna";
        outfileN[0] = temp_dir + "/input_N.dna";
        outfileN[1] = temp_dir + "/input_N.dna.2";
        outfileorderN[0] = temp_dir + "/read_order_N.bin";
        outfileorderN[1] = temp_dir + "/read_order_N.bin.2";
        outfileid = temp_dir + "/id_1";
        outfilequality[0] = temp_dir + "/quality_1";
        outfilequality[1] = temp_dir + "/quality_2";
    }

    void preprocess(core::record::Chunk&& t, size_t id);

    void finish();
};

}  // namespace spring

}  // namespace read
}  // namespace genie

#endif  // SPRING_PREPROCESS_H_
