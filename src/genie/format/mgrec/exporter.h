/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGREC_EXPORTER_H_
#define SRC_GENIE_FORMAT_MGREC_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/format_exporter.h"
#include "genie/core/record/alignment/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bit_writer.h"
#include "genie/util/drain.h"
#include "genie/util/ordered_lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

/**
 * @brief
 */
class Exporter : public core::FormatExporter {
    util::BitWriter writer;  //!< @brief
    util::OrderedLock lock;  //!< @brief

 public:
    /**
     * @brief
     * @param _file_1
     */
    explicit Exporter(std::ostream& _file_1);

    /**
     * @brief
     * @param t
     * @param id
     */
    void FlowIn(core::record::Chunk&& t, const util::Section& id) override;

    /**
     * @brief
     * @param id
     */
    void SkipIn(const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGREC_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
