/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_EXPORTER_H
#define GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/chunk.h>
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
    util::BitWriter writer;  //!<
    util::OrderedLock lock;  //!<

   public:
    /**
     *
     * @param _file_1
     */
    explicit MgrecsExporter(std::ostream& _file_1);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
