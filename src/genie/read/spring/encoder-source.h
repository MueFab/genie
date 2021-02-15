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
 *
 */
class SpringSource : public util::OriginalSource, public util::Source<core::AccessUnit> {
 private:
    uint32_t num_AUs;                                    //!<
    std::string read_desc_prefix;                        //!<
    std::string id_desc_prefix;                          //!<
    std::string quality_desc_prefix;                     //!<
    std::vector<uint32_t> num_records_per_AU;            //!<
    std::vector<uint32_t> num_reads_per_AU;              //!<
    uint32_t auId;                                       //!<
    std::vector<core::parameter::ParameterSet>& params;  //!<

    core::stats::PerfStats stats;  //!<

 public:
    /**
     *
     * @param temp_dir
     * @param cp
     * @param p
     * @param s
     */
    SpringSource(const std::string& temp_dir, const compression_params& cp,
                 std::vector<core::parameter::ParameterSet>& p, core::stats::PerfStats s);

    /**
     *
     * @param id
     * @param lock
     * @return
     */
    bool pump(uint64_t& id, std::mutex& lock) override;

    /**
     *
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
