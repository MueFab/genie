/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGREC_EXPORTER_H_
#define SRC_GENIE_FORMAT_MGREC_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/format-exporter.h"
#include "genie/core/record/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bit-writer.h"
#include "genie/util/drain.h"
#include "genie/util/ordered-lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgrec {

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
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;

    /**
     * @brief
     * @param id
     */
    void skipIn(const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGREC_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
