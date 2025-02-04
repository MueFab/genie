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
#include "genie/util/ordered_lock.h"
#include "genie/util/ordered_section.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 * @brief
 */
struct Preprocessor {
    compression_params cp;  //!< @brief

    std::string outfileclean[2];       //!< @brief
    std::string outfileN[2];           //!< @brief
    std::string outfileorderN[2];      //!< @brief
    std::string outfileid;             //!< @brief
    std::string outfilequality[2];     //!< @brief
    std::string outfilereadlength[2];  //!< @brief

    std::ofstream fout_clean[2];    //!< @brief
    std::ofstream fout_N[2];        //!< @brief
    std::ofstream fout_order_N[2];  //!< @brief
    std::ofstream fout_id;          //!< @brief
    std::ofstream fout_quality[2];  //!< @brief

    std::string temp_dir;     //!< @brief
    std::string working_dir;  //!< @brief

    util::OrderedLock lock;  //!< @brief

    core::stats::PerfStats stats;  //!< @brief

    /**
     * @brief
     * @return
     */
    core::stats::PerfStats& getStats();

    bool used = false;  //!< @brief

    /**
     * @brief
     * @param working_dir
     * @param num_thr
     * @param paired_end
     */
    void setup(const std::string& working_dir, size_t num_thr, bool paired_end);

    /**
     * @brief
     */
    ~Preprocessor();

    /**
     * @brief
     * @param t
     * @param id
     */
    void preprocess(core::record::Chunk&& t, const util::Section& id);

    /**
     * @brief
     * @param id
     */
    void skip(const util::Section& id);

    /**
     * @brief
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
