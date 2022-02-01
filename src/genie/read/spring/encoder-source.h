/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_ENCODER_SOURCE_H_
#define SRC_GENIE_READ_SPRING_ENCODER_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/read/spring/util.h"
#include "genie/util/original-source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 * @brief
 */
class SpringSource : public util::OriginalSource, public util::Source<core::AccessUnit> {
 private:
    uint32_t num_AUs;                                   //!< @brief
    std::string read_desc_prefix;                       //!< @brief
    std::string id_desc_prefix;                         //!< @brief
    std::string quality_desc_prefix;                    //!< @brief
    std::vector<uint32_t> num_records_per_AU;           //!< @brief
    std::vector<uint32_t> num_reads_per_AU;             //!< @brief
    uint32_t auId;                                      //!< @brief
    std::vector<core::parameter::EncodingSet>& params;  //!< @brief

    core::stats::PerfStats stats;  //!< @brief

 public:
    /**
     * @brief
     * @param temp_dir
     * @param cp
     * @param p
     * @param s
     */
    SpringSource(const std::string& temp_dir, const compression_params& cp,
                 std::vector<core::parameter::EncodingSet>& p, core::stats::PerfStats s);

    /**
     * @brief
     * @param id
     * @param lock
     * @return
     */
    bool pump(uint64_t& id, std::mutex& lock) override;

    /**
     * @brief
     * @param pos
     */
    void flushIn(uint64_t& pos) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_ENCODER_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
