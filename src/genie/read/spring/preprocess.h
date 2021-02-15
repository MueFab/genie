/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_PREPROCESS_H_
#define SRC_GENIE_READ_SPRING_PREPROCESS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "filesystem/filesystem.hpp"
#include "genie/core/record/chunk.h"
#include "genie/read/spring/util.h"
#include "genie/util/drain.h"
#include "genie/util/ordered-lock.h"
#include "genie/util/ordered-section.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 *
 */
struct Preprocessor {
    compression_params cp;  //!<

    std::string outfileclean[2];       //!<
    std::string outfileN[2];           //!<
    std::string outfileorderN[2];      //!<
    std::string outfileid;             //!<
    std::string outfilequality[2];     //!<
    std::string outfilereadlength[2];  //!<

    std::ofstream fout_clean[2];    //!<
    std::ofstream fout_N[2];        //!<
    std::ofstream fout_order_N[2];  //!<
    std::ofstream fout_id;          //!<
    std::ofstream fout_quality[2];  //!<

    std::string temp_dir;     //!<
    std::string working_dir;  //!<

    util::OrderedLock lock;  //!<

    core::stats::PerfStats stats;  //!<

    /**
     *
     * @return
     */
    core::stats::PerfStats& getStats();

    bool used = false;  //!<

    /**
     *
     * @param working_dir
     * @param num_thr
     * @param paired_end
     */
    void setup(const std::string& working_dir, size_t num_thr, bool paired_end);

    /**
     *
     */
    ~Preprocessor();

    /**
     *
     * @param t
     * @param id
     */
    void preprocess(core::record::Chunk&& t, const util::Section& id);

    /**
     *
     * @param id
     */
    void skip(const util::Section& id);

    /**
     *
     * @param pos
     */
    void finish(size_t pos);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_PREPROCESS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
