/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_PREPROCESS_H_
#define SPRING_PREPROCESS_H_

#include <filesystem/filesystem.hpp>
#include <string>
#include "util.h"

#include <genie/core/record/chunk.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/ordered-section.h>
#include <genie/util/drain.h>

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
    std::string working_dir;

    util::OrderedLock lock;

    core::stats::PerfStats stats;

    core::stats::PerfStats& gteStats() {
        return stats;
    }

    bool used = false;

    void setup(const std::string& working_dir, size_t num_thr, bool paired_end);

    ~Preprocessor(){
        if(!used) {
            for (int j = 0; j < 2; j++) {
                if (j == 1 && !cp.paired_end) continue;
                fout_clean[j].close();
                fout_N[j].close();
                fout_order_N[j].close();
                if (cp.preserve_quality) fout_quality[j].close();
            }
            if (cp.preserve_id) fout_id.close();

            for (int j = 0; j < 2; j++) {
                if (j == 1 && !cp.paired_end) continue;
                ghc::filesystem::remove(outfileclean[j]);
                ghc::filesystem::remove(outfileN[j]);
                ghc::filesystem::remove(outfileorderN[j]);
                if (cp.preserve_quality) ghc::filesystem::remove(outfilequality[j]);
            }
            if (cp.preserve_id) ghc::filesystem::remove(outfileid);

            ghc::filesystem::remove(temp_dir);
        }
    }

    void preprocess(core::record::Chunk&& t, const util::Section& id);

    void skip(const util::Section& id) {
        util::OrderedSection sec(&lock, id);
    }

    void finish(size_t pos);


};

}  // namespace spring

}  // namespace read
}  // namespace genie

#endif  // SPRING_PREPROCESS_H_
