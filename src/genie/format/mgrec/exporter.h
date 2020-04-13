/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_EXPORTER_H
#define GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/bitwriter.h>
#include <genie/util/drain.h>
#include <genie/util/ordered-lock.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

/**
 *
 */
class MgrecsExporter : public util::Drain<core::record::Chunk> {
    util::BitWriter writer;              //!<
    util::OrderedLock lock;              //!<
    genie::core::stats::PerfStats *stats; //!< @brief Stats collector (null => don't collect)

   public:
    /**
     *
     * @param _file_1
     */
    explicit MgrecsExporter(std::ostream& _file_1,  genie::core::stats::PerfStats *_stats = nullptr);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(core::record::Chunk&& t, size_t id) override;

    /**
     *
     */
    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
