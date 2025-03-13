/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_ENCODER_H_
#define SRC_GENIE_READ_SPRING_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/read_encoder.h"
#include "genie/core/stats/perf_stats.h"
#include "genie/read/spring/preprocess.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 * @brief
 */
class Encoder : public genie::core::ReadEncoder {
 private:
    Preprocessor preprocessor;  //!< @brief

 public:
    /**
     * @brief
     * @param working_dir
     * @param num_thr
     * @param paired_end
     * @param write_raw
     */
    explicit Encoder(const std::string& working_dir, size_t num_thr, bool paired_end, bool write_raw);

    /**
     * @brief
     * @param t
     * @param id
     */
    void FlowIn(genie::core::record::Chunk&& t, const util::Section& id) override;

    /**
     * @brief
     * @param pos
     */
    void FlushIn(uint64_t& pos) override;

    /**
     * @brief
     * @param id
     */
    void SkipIn(const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
